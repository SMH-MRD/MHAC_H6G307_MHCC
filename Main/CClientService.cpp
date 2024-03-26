#include "CClientService.h"
#include "OTE0panel.h"
#include "PLC_DEF.h"


//-共有メモリオブジェクトポインタ:
extern CSharedMem* pCraneStatusObj;
extern CSharedMem* pSwayStatusObj;
extern CSharedMem* pPLCioObj;
extern CSharedMem* pSwayIO_Obj;
extern CSharedMem* pOTEioObj;
extern CSharedMem* pCSInfObj;
extern CSharedMem* pPolicyInfsObj;
extern CSharedMem* pAgentInfObj;
extern CSharedMem* pJobIO_Obj;

extern vector<void*>	VectpCTaskObj;	//タスクオブジェクトのポインタ
extern ST_iTask g_itask;
extern ST_SPEC spec;

ERMPLC_BOUT_MAP plc_bo_map;	//PLC入力信号解析用
CABPLC_BOUT_MAP plc_bi_map;	//PLC入力信号解析用

/****************************************************************************/
/*   コンストラクタ　デストラクタ                                           */
/****************************************************************************/
CClientService::CClientService() {
	pPLC_IO = NULL;
	pCraneStat = NULL;
}

CClientService::~CClientService() {

}
/****************************************************************************/
/*   タスク初期化処理                                                       */
/* 　メインスレッドでインスタンス化した後に呼びます。                       */
/****************************************************************************/
//static BOOL PLC_PBs_last[N_PLC_PB];

void CClientService::init_task(void* pobj) {

	//共有メモリ構造体のポインタセット
	pPLC_IO = (LPST_PLC_IO)(pPLCioObj->get_pMap());
	pCraneStat = (LPST_CRANE_STATUS)(pCraneStatusObj->get_pMap()); 
	pCSinf = (LPST_CS_INFO)(pCSInfObj->get_pMap());
	pAgent_Inf = (LPST_AGENT_INFO)(pAgentInfObj->get_pMap());
	pOTE_IO = (LPST_OTE_IO)(pOTEioObj->get_pMap());
	pJob_IO = (LPST_JOB_IO)(pJobIO_Obj->get_pMap());

	for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
		if (i == ID_OTE_GRIP_SWITCH)continue;//	グリップスイッチは対象外
		CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0ノッチで初期化
		//ランプ指令
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].com = OTE_LAMP_COM_ON;
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].color = OTE0_GREEN;
	}

	CS_workbuf.ote_remote_status &= ~CS_CODE_OTE_REMOTE_ENABLE; //リモート操作可をクリア

	for (int i = ID_HOIST; i <= ID_AHOIST; i++) CS_workbuf.auto_status[i] = STAT_MANUAL;

	return;
};

/****************************************************************************/
/*   タスク定周期処理                                                       */
/* 　タスクスレッドで毎周期実行される関数			　                      */
/****************************************************************************/

static UINT16 pb_ope_last[N_OTE_PNL_PB];
static INT32 ote_grip_last;

void CClientService::routine_work(void* param) {
	input();
	main_proc();
	output();

	//操作PB前回値保持
	for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_PB_FUREDOME; i++) {
		pb_ope_last[i] = pOTE_IO->ote_umsg_in.body.pb_ope[i];
	}
	//グリップスイッチ前回値保持
	INT32 ote_grip_last = pOTE_IO->ote_grip;
};

//# 定周期処理手順1　外部信号入力
/***  JOB関連入力信号処理（自動開始PB)   ***/
/****************************************************************************/
/*  入力処理																*/
/****************************************************************************/

void CClientService::input() {

	return;
};

//# OTE入力操作端末有効判断
int CClientService::can_ote_activate() {
	if ((pOTE_IO->ote_u_silent_cnt < CS_OTE_U_MSG_TIMEOUT)&&(pOTE_IO->ote_umsg_in.body.ope_mode & OTE_ID_OPE_MODE_COMMAND)) {
		CS_workbuf.ote_remote_status |= CS_CODE_OTE_REMOTE_ENABLE;
		return L_ON;
	}
	CS_workbuf.ote_remote_status &= ~CS_CODE_OTE_REMOTE_ENABLE;
	return L_OFF;
}
/****************************************************************************/
/*  メイン処理																*/
/****************************************************************************/
int CClientService::ote_handle_proc() {         //操作端末処理
	if (can_ote_activate()) {
		//グリップスイッチ
		{
			CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_RMT].com = CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_PAD_MODE].com = CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_ESTOP].com = CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_NOTCH].com = OTE_LAMP_COM_ON;

			if (pOTE_IO->ote_estop == L_OFF)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_ESTOP].color = OTE0_GREEN;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_ESTOP].color = OTE0_RED;

			if (pOTE_IO->ote_grip == L_ON)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_NOTCH].color = OTE0_ORANGE;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_NOTCH].color = OTE0_GREEN;

			if (pOTE_IO->ote_padmode == L_ON)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_PAD_MODE].color = OTE0_ORANGE;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_PAD_MODE].color = OTE0_GLAY;

			if (pOTE_IO->ote_umsg_in.body.ope_mode & OTE_ID_OPE_MODE_COMMAND)
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_RMT].color = OTE0_ORANGE;
			else
				CS_workbuf.ote_notch_lamp[ID_OTE_GRIP_RMT].color = OTE0_GLAY;
		}
		//操作スイッチ
		{
			//停止　PBL
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].com = CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].com = OTE_LAMP_COM_ON;
			if (pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_TEISHI]) CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].color = OTE0_GREEN;
			else													CS_workbuf.ote_pb_lamp[ID_OTE_PB_TEISHI].color = OTE0_GLAY;
			//起動 PBL
			if (pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_KIDOU]) CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].color = OTE0_RED;
			else													CS_workbuf.ote_pb_lamp[ID_OTE_PB_KIDOU].color = OTE0_GLAY;

			//自動　PBL
			if ((pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_AUTO]) && !(pb_ope_last[ID_OTE_PB_AUTO])) {
				if (CS_workbuf.auto_mode)	CS_workbuf.auto_mode = L_OFF;
				else 						CS_workbuf.auto_mode = L_ON;
			}
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_AUTO].com = OTE_LAMP_COM_ON;
			if (CS_workbuf.auto_mode)	CS_workbuf.ote_pb_lamp[ID_OTE_PB_AUTO].color = OTE0_ORANGE;
			else 						CS_workbuf.ote_pb_lamp[ID_OTE_PB_AUTO].color = OTE0_GREEN;

			//振れ止め　PBL
			if ((pOTE_IO->ote_umsg_in.body.pb_ope[ID_OTE_PB_FUREDOME]) && !(pb_ope_last[ID_OTE_PB_FUREDOME])) {
				if (CS_workbuf.antisway_mode)	CS_workbuf.antisway_mode = L_OFF;
				else							CS_workbuf.antisway_mode = L_ON;
			}
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_FUREDOME].com = OTE_LAMP_COM_ON;
			if (CS_workbuf.antisway_mode)	CS_workbuf.ote_pb_lamp[ID_OTE_PB_FUREDOME].color = OTE0_ORANGE;
			else							CS_workbuf.ote_pb_lamp[ID_OTE_PB_FUREDOME].color = OTE0_GREEN;

			//自動選択状態
			for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
				CS_workbuf.auto_status[i] = pOTE_IO->ote_umsg_in.body.auto_sel[i];
			}

		}
		//ランプ
		{
			//主幹ランプ　PB受け付け処理はPLC IF
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_SYUKAN].com = OTE_LAMP_COM_ON;
			if (pPLC_IO->input.rbuf.erm_bo[plc_bo_map.ctrl_source_mc_ok.x] & plc_bo_map.ctrl_source_mc_ok.y) {

				CS_workbuf.ote_pb_lamp[ID_OTE_PB_SYUKAN].color = OTE0_RED;
			}
			else {
				CS_workbuf.ote_pb_lamp[ID_OTE_PB_SYUKAN].color = OTE0_GREEN;
			}
			//非常停止ランプ　PB受け付け処理はPLC IF
			CS_workbuf.ote_pb_lamp[ID_OTE_PB_HIJYOU].com = OTE_LAMP_COM_ON;
			if (pPLC_IO->input.rbuf.cab_bi[plc_bi_map.cab_estp.x] & plc_bi_map.cab_estp.y) {

				CS_workbuf.ote_pb_lamp[ID_OTE_PB_HIJYOU].color = OTE0_GREEN;
			}
			else {
				CS_workbuf.ote_pb_lamp[ID_OTE_PB_HIJYOU].color = OTE0_RED;
			}



			//ノッチランプ
			for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
				if (i == ID_OTE_GRIP_SWITCH)continue;//	グリップスイッチは対象外
				for (int j = 0; j < 9; j++) {
					CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_ON;

					if (j == pOTE_IO->ote_umsg_in.body.notch_pos[ID_OTE_NOTCH_POS_HOLD][i]) {
						CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_ON;

						if (j == ID_OTE_0NOTCH_POS)	CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GREEN;
						else						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_ORANGE;
					}
					else {

						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GLAY;
					}
				}


				for (int i = ID_HOIST; i <= ID_AHOIST; i++) {

					if (i == ID_OTE_GRIP_SWITCH) { continue; }//	グリップスイッチは対象外

					//全クリア
					for (int j = 0; j < 9; j++) {
						CS_workbuf.ote_notch_lamp[i * 10 + j].com = OTE_LAMP_COM_ON;
						CS_workbuf.ote_notch_lamp[i * 10 + j].color = OTE0_GLAY;
					}

					int index_base = i * 10 + NOTCH_4;	//ノッチは10個の配列　0ノッチは4の位置

					//ノッチ指令表示
					if (pPLC_IO->v_com_notch[i] != 0)	//0ﾉｯﾁ以外
						CS_workbuf.ote_notch_lamp[pPLC_IO->v_com_notch[i] + index_base].color = OTE0_ORANGE;
					//ノッチ速度FB表示
					CS_workbuf.ote_notch_lamp[pPLC_IO->v_fb_notch[i] + index_base].color = OTE0_RED;
					//0ノッチ表示
					if(pPLC_IO->v_com_notch[i] == 0) CS_workbuf.ote_notch_lamp[index_base].color = OTE0_GREEN;
				}
			}
		}
	}
	else {//ランプOFF
		//操作ランプ
		for (int i = ID_OTE_PB_TEISHI; i <= ID_OTE_PB_FUREDOME; i++) {
			CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_OFF;
		}
		//ノッチランプ
		for (int i = 0; i < N_OTE_PNL_NOTCH; i++) {
			CS_workbuf.ote_notch_lamp[i].com = OTE_LAMP_COM_OFF;
		}

		for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
			if (i == ID_OTE_GRIP_SWITCH)continue;//	グリップスイッチは対象外
			CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0ノッチで初期化
			//ランプ指令
			CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].com = OTE_LAMP_COM_ON;
			CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].color = OTE0_GREEN;
		}
	}

	return 0; 
}

static UINT32 ote_target_seq_last;
void CClientService::main_proc() {

	//操作端末処理
	ote_handle_proc();
	
	//＃＃＃ジョブイベント処理
	//半自動登録処理

	LPST_JOB_SET p_job;

	if ((CS_workbuf.auto_mode == L_OFF) &&(CS_workbuf.job_control_status != CS_JOBSET_STATUS_DISABLE)){
		pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job		= pJob_IO->job_list[ID_JOBTYPE_JOB].n_job		= 0;
		pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot	= pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot	= 0;
		CS_workbuf.job_control_status = CS_JOBSET_STATUS_DISABLE;
	}

	//イベント処理
	switch (CS_workbuf.job_control_status) {

	case CS_JOBSET_STATUS_DISABLE: {
		if(CS_workbuf.auto_mode==L_ON) CS_workbuf.job_control_status = CS_JOBSET_STATUS_IDLE;
		break;
	}
	case CS_JOBSET_STATUS_IDLE:		//ジョブ無し
	case CS_JOBSET_STATUS_STANDBY:	//ジョブ有り
	{
		p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot];

		//半自動コマンド受け付け処理
		if ((pOTE_IO->ote_grip == L_ON) && (ote_grip_last == L_OFF)) {//グリップトリガ
			//目標位置シーケンス番号更新あればコマンド受付　JOB登録
			if (pOTE_IO->ote_umsg_in.body.target_seq_no != ote_target_seq_last) {
				//JOB LIST処理
				pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job		= 1;
				pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot	= 0;//半自動はバッファ固定
				pJob_IO->job_list[ID_JOBTYPE_SEMI].status[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot]	= CS_JOBSET_STATUS_STANDBY;
				
				//JOB SET処理
				p_job->status		= STAT_TRIGED;
				p_job->list_id		= ID_JOBTYPE_SEMI;
				p_job->n_com		= 1;//JOBのコマンド数　半自動は１	
				p_job->job_id		= 0;

				p_job->type			= ID_JOBTYPE_SEMI;
				p_job->code			= pOTE_IO->ote_umsg_in.body.target_seq_no;
				p_job->com_type[0]	= ID_JOBIO_COMTYPE_PARK;
				//目標位置セット
				p_job->targets[0].pos[ID_HOIST]		= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_HOIST];
				p_job->targets[0].pos[ID_BOOM_H]	= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_BOOM_H];
				p_job->targets[0].pos[ID_SLEW]		= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_SLEW];
				p_job->targets[0].pos[ID_AHOIST]	= pOTE_IO->ote_umsg_in.body.auto_tg_pos[ID_AHOIST];

				CS_workbuf.job_control_status = CS_JOBSET_STATUS_STANDBY;

				//クライアントへ報告
				wostrs.str(L"");
				wostrs << L"移動コマンド受け付けました　No.=" << pOTE_IO->ote_umsg_in.body.target_seq_no;
				txout2msg_listbox(wostrs.str());

				ote_target_seq_last = pOTE_IO->ote_umsg_in.body.target_seq_no;
			}
		}

		//ジョブが完了していたらIDLE状態
		if ((CS_workbuf.job_control_status == CS_JOBSET_STATUS_STANDBY) && (p_job->status == STAT_END)) {
			CS_workbuf.job_control_status = CS_JOBSET_STATUS_IDLE;
			//p_jobのフラグ類はPOLICYのステータス更新呼び出しで更新
		}

	}break;

	default:break;
	}

	
	//現在アクティブなJOB
	if (CS_workbuf.auto_mode == L_OFF) {
		CS_workbuf.p_active_job = NULL;
	}
	else if (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job != 0) {
		p_job = &(pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot]);
		switch (p_job->status) {
		case STAT_TRIGED://起動待ち
		case STAT_ACTIVE:
		case STAT_STANDBY:
			CS_workbuf.p_active_job = p_job;
			break;

		case STAT_SUSPENDED:
		case STAT_ABOTED:
		case STAT_END:
		case STAT_ABNORMAL_END:
			CS_workbuf.p_active_job = NULL;
			break;
		}
	}
	else if (pJob_IO->job_list[ID_JOBTYPE_JOB].n_job != 0) {
		p_job = &(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot]);
		switch (p_job->status) {
		case STAT_TRIGED://起動待ち
		case STAT_ACTIVE:
		case STAT_STANDBY:
			CS_workbuf.p_active_job = p_job;
			break;

		case STAT_SUSPENDED:
		case STAT_ABOTED:
		case STAT_END:
		case STAT_REQ_WAIT:
			CS_workbuf.p_active_job = NULL;
			break;
		}
	}
	else {
		CS_workbuf.p_active_job = NULL;
	}

	//グリップスイッチ状態の判定（AGENT用）JOB判定後の状態を渡すため
	grip_status = pOTE_IO->ote_grip;

	return;
}

//ジョブのレシピセット
LPST_JOB_SET CClientService::set_job_seq(LPST_JOB_SET pjob_set) {
	LPST_JOB_SET pjob = NULL;
	return pjob;
}

//半自動のレシピセット
LPST_JOB_SET CClientService::set_semi_seq(LPST_JOB_SET pjob_set) {

	return pjob_set;
}

int CClientService::perce_client_message(LPST_CLIENT_COM_RCV_MSG pmsg) {
	return 0;
}

/****************************************************************************/
/*  出力処理																*/
/*  ジョブ関連ランプ表示他													*/
/****************************************************************************/
void CClientService::output() {
	
	//共有メモリ出力
	memcpy_s(pCSinf, sizeof(ST_CS_INFO), &CS_workbuf, sizeof(ST_CS_INFO));
	//タスクパネル表示出力
	{
		
		wostrs.str(L"");
		wostrs << L" AS=" << CS_workbuf.antisway_mode << L",AUTO=" << CS_workbuf.auto_mode;

		int status;
		if (CS_workbuf.p_active_job != NULL) {
			status = CS_workbuf.p_active_job->status;
			wostrs << L",JOB TYPE=" << CS_workbuf.p_active_job->list_id << L", id=" << CS_workbuf.p_active_job->job_id;
		}
		else status = STAT_NA;

		if ((pJob_IO->job_list[ID_JOBTYPE_JOB].n_job <= 0) && (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job <= 0)) {
			wostrs << L" >JOB: 0 hold";
		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_JOB) &&
			(pJob_IO->job_list[ID_JOBTYPE_JOB].n_job > 0)) {
			wostrs << L" >JOB: " << pJob_IO->job_list[ID_JOBTYPE_JOB].n_job << L" iHOT-> ";
			if (status & STAT_STANDBY)			wostrs << L"STANDBY";
			else if (status & STAT_ACTIVE)		wostrs << L"ACTIVE";
			else if (status & STAT_TRIGED)		wostrs << L"TRIGED";
			else if (status & STAT_SUSPENDED)	wostrs << L"SUSPEND";
			else if (status & STAT_REQ_WAIT)	wostrs << L"WAIT REQ";
			else wostrs << L"OUT OF SERV";

		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_SEMI) &&
			(pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job > 0)) {
			wostrs << L" >SEMIAUTO: " << pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job << L" iHOT-> ";
			if (status & STAT_STANDBY)		wostrs << L"STANDBY";
			else if (status & STAT_ACTIVE)	wostrs << L"ACTIVE";
			else if (status & STAT_TRIGED)	wostrs << L"TRIGED";
			else if (status & STAT_SUSPENDED)	wostrs << L"SUSPEND";
			else if (status & STAT_REQ_WAIT)	wostrs << L"WAIT REQ";
			else wostrs << L"OUT OF SERV";
		}
		else  wostrs << L" >NO JOB REQUEST ";

		wostrs << L" >OTEU " << pOTE_IO->ote_u_silent_cnt;

		wostrs << L" --Scan " << inf.period;

		tweet2owner(wostrs.str()); wostrs.str(L""); wostrs.clear();
	}

	return;

};

/****************************************************************************/
/*   他タスクからのアクセス関数												*/
/****************************************************************************/

/// <summary>
/// AGENTからの実行待ちJOB問い合わせ応答
/// </summary>
/// <returns>LPST_JOB_SET : ジョブセット構造体のポインタ</returns>
LPST_JOB_SET CClientService::get_next_job() {
	
	return CS_workbuf.p_active_job;
}

//### POLICYからのJOB Status更新依頼
int CClientService::update_job_status(LPST_JOB_SET pjobset, int fb_code) {

	if (pjobset->list_id == ID_JOBTYPE_JOB) {
		switch (fb_code) {
		case STAT_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
			break;
		}
		case STAT_ABNORMAL_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_ABNORMAL_END);

			return STAT_ACK;
		}break;
		case STAT_ACTIVE :break;
		case STAT_ABOTED:break;
		default:break;
		}
	}
	else if (pjobset->list_id == ID_JOBTYPE_SEMI) {//半自動ではコマンド完了＝ジョブ完了
		switch (fb_code) {
		case STAT_END: {
			//正常完了時JOBのホールド数を0クリア
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job = 0;
			pjobset->status = STAT_END;

			//実行中ジョブ解除
			CS_workbuf.p_active_job = NULL;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
		}break;
		case STAT_ABNORMAL_END: {
			//異常完了時JOBのホールド数を0クリア
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job = 0;
			pjobset->status = STAT_ABNORMAL_END;

			//実行中ジョブ解除
			CS_workbuf.p_active_job = NULL;

			job_report2client(pjobset, STAT_ABNORMAL_END);

			return STAT_ACK;
		}break;

		case STAT_ABOTED: {
			//ジョブキャンセル時（自動OFF,グリップスイッチOFF等）JOBのホールド数を0クリア
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_job = 0;
			pjobset->status = STAT_ABOTED;

			//実行中ジョブ解除
			CS_workbuf.p_active_job = NULL;

			job_report2client(pjobset, STAT_ABOTED);

			return STAT_ACK;
		}break;

		case STAT_STANDBY: {
			//ジョブ実行報告は、ステータスの更新のみ
			pjobset->status = STAT_STANDBY;

			job_report2client(pjobset, STAT_STANDBY);

			return STAT_ACK;
		}break;

		case STAT_ACTIVE: {
			//ジョブ実行報告は、ステータスの更新のみ
			pjobset->status = STAT_ACTIVE;

			job_report2client(pjobset, STAT_ACTIVE);

			return STAT_ACK;
		}break;		
		
		case STAT_SUSPENDED: {
			//中断時（手動介入）の場合は、ステータスをスタンバイ状態に戻す
			pjobset->status = STAT_SUSPENDED;

			job_report2client(pjobset, STAT_SUSPENDED);

			return STAT_ACK;
		}break;
		default:return STAT_LOGICAL_ERROR;
		}
	}

	return STAT_LOGICAL_ERROR;
}



/****************************************************************************/
/*   JOB制御															　　*/
/****************************************************************************/

//クライアントへのフィードバック
int CClientService::job_report2client(LPST_JOB_SET pjobset, int fb_code) {       //Jobの実行状況報告

	if (pjobset == NULL)return 0;

	//クライアントへ報告
	wostrs.clear(); wostrs.str(L"");
	wostrs << L"コマンドNo.=" << pjobset->code << L":";

	if (pjobset->list_id == ID_JOBTYPE_JOB) {
		switch (fb_code) {
		case STAT_END: {
		}break;
		case STAT_ABNORMAL_END:break;
		case STAT_ACTIVE:break;
		case STAT_ABOTED:break;
		default:break;
		}
	}
	else if (pjobset->list_id == ID_JOBTYPE_SEMI) {
		switch (fb_code) {
		case STAT_END: {
			wostrs << L"正常完了";
		}break;
		case STAT_ABNORMAL_END: {
			wostrs << L"異常完了";
		}break;
		case STAT_STANDBY: {
			wostrs << L"コマンド" << pjobset->i_hot_com << L"準備完";
		}break;
		case STAT_ACTIVE: {
			wostrs << L"コマンド" << pjobset->i_hot_com << L"起動";
		}break;
		case STAT_ABOTED: {
			wostrs << L"キャンセル";
		}break;
		case STAT_SUSPENDED: {
			wostrs << L"中断中";
		}break;
		default:break;
		}
	}
	txout2msg_listbox(wostrs.str());

	return STAT_SUCCEED;
	return STAT_NA;
}


void CClientService::init_job() {
	return;
}

/****************************************************************************/
/*   タスク設定タブパネルウィンドウのコールバック関数                       */
/****************************************************************************/
LRESULT CALLBACK CClientService::PanelProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_TASK_FUNC_RADIO1:
		case IDC_TASK_FUNC_RADIO2:
		case IDC_TASK_FUNC_RADIO3:
		case IDC_TASK_FUNC_RADIO4:
		case IDC_TASK_FUNC_RADIO5:
		case IDC_TASK_FUNC_RADIO6:
			inf.panel_func_id = LOWORD(wp); set_panel_tip_txt(); set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0); 
			reset_panel_item_pb(hDlg);
			break;

		case IDC_TASK_ITEM_RADIO1:
		case IDC_TASK_ITEM_RADIO2:
		case IDC_TASK_ITEM_RADIO3:
		case IDC_TASK_ITEM_RADIO4:
		case IDC_TASK_ITEM_RADIO5:
		case IDC_TASK_ITEM_RADIO6:
			inf.panel_type_id = LOWORD(wp);set_panel_tip_txt();  SetFocus(GetDlgItem(inf.hWnd_opepane, IDC_TASK_EDIT1));
			if (inf.panel_func_id == IDC_TASK_FUNC_RADIO6) {
				if (inf.panel_type_id == IDC_TASK_ITEM_RADIO1) {
					;
				}
			}
			break;
		case IDSET: {
			wstring wstr, wstr_tmp;

			//サンプルとしていろいろな型で読み込んで表示している
			wstr += L"Param 1(d):";
			int n = GetDlgItemText(hDlg, IDC_TASK_EDIT1, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stod(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 2(i):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT2, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stoi(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 3(f):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT3, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stof(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 4(l):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT4, (LPTSTR)wstr_tmp.c_str(), 128);
			if (n) wstr_tmp = to_wstring(stol(wstr_tmp));	wstr = wstr + wstr_tmp.c_str();

			wstr += L",  Param 5(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT5, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			wstr += L",   Param 6(c):";
			n = GetDlgItemText(hDlg, IDC_TASK_EDIT6, (LPTSTR)wstr_tmp.c_str(), 128);
			wstr += wstr_tmp.c_str();

			txout2msg_listbox(wstr);


		}break;
		case IDRESET: {
			set_PNLparam_value(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
			reset_panel_func_pb(hDlg);
		}break;

		case IDC_TASK_OPTION_CHECK1:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK2), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK1) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION1;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;

		case IDC_TASK_OPTION_CHECK2:
			SendMessage(GetDlgItem(hDlg, IDC_TASK_OPTION_CHECK1), BM_SETCHECK, BST_UNCHECKED, 0L);
			if (IsDlgButtonChecked(hDlg, IDC_TASK_OPTION_CHECK2) == BST_CHECKED) inf.work_select = THREAD_WORK_OPTION2;
			else inf.work_select = THREAD_WORK_ROUTINE;
			break;
		}
	}
	return 0;
};

/****************************************************************************/
/*   タスク設定パネルの操作ボタン説明テキスト設定関数                       */
/****************************************************************************/
void CClientService::set_panel_tip_txt()
{
	wstring wstr_type; wstring wstr;
	switch (inf.panel_func_id) {
	case IDC_TASK_FUNC_RADIO1: {
		wstr = L"Type for Func1 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO2: {
		wstr = L"Type of Func2 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO3: {
		wstr = L"Type for Func3 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO4: {
		wstr = L"Type for Func4 \n\r 1:VP 2 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO5: {
		wstr = L"Type for Func5 \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	case IDC_TASK_FUNC_RADIO6: {
		wstr = L"Func6(Debug) \n\r 1:SIM 2:PLC 3:SWAY 4:RIO 5:?? 6:??";
		switch (inf.panel_type_id) {
		case IDC_TASK_ITEM_RADIO1:
			wstr_type += L"Param of type1 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO2:
			wstr_type += L"Param of type2 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO3:
			wstr_type += L"Param of type3 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO4:
			wstr_type += L"Param of type4 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO5:
			wstr_type += L"Param of type5 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		case IDC_TASK_ITEM_RADIO6:
			wstr_type += L"Param of type6 \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
			break;
		default:break;
		}
	}break;
	default: {
		wstr = L"Type for Func? \n\r 1:?? 2:?? 3:?? \n\r 4:?? 5:?? 6:??";
		wstr_type += L"(Param of type?) \n\r 1:?? 2:??  3:?? \n\r 4:?? 5:?? 6:??";
	}break;
	}

	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET2), wstr.c_str());
	SetWindowText(GetDlgItem(inf.hWnd_opepane, IDC_STATIC_TASKSET3), wstr_type.c_str());
}

/****************************************************************************/
/*　　タスク設定パネルボタンのテキストセット					            */
/****************************************************************************/
void CClientService::set_panel_pb_txt() {

	//WCHAR str_func06[] = L"DEBUG";

	//SetDlgItemText(inf.hWnd_opepane, IDC_TASK_FUNC_RADIO6, (LPCWSTR)str_func06);

	return;
};
