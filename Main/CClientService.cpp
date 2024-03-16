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

	for (int i = 0; i < SEMI_AUTO_TARGET_MAX; i++) {
		CS_workbuf.semi_auto_setting_target[i].pos[ID_HOIST] = spec.semi_target[i][ID_HOIST];
		CS_workbuf.semi_auto_setting_target[i].pos[ID_BOOM_H] = spec.semi_target[i][ID_BOOM_H];
		CS_workbuf.semi_auto_setting_target[i].pos[ID_SLEW] = spec.semi_target[i][ID_SLEW];
	}

	for (int i = ID_HOIST; i <= ID_AHOIST; i++) {
		if (i == ID_OTE_GRIP_SWITCH)continue;//	グリップスイッチは対象外
		CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_HOLD][i] = CS_workbuf.notch_pos[ID_OTE_NOTCH_POS_TRIG][i] = ID_OTE_0NOTCH_POS;	//0ノッチで初期化
		//ランプ指令
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].com = OTE_LAMP_COM_ON;
		CS_workbuf.ote_notch_lamp[i * 10 + ID_OTE_0NOTCH_POS].color = OTE0_GREEN;
	}

	CS_workbuf.ote_remote_status &= ~CS_CODE_OTE_REMOTE_ENABLE; //リモート操作可をクリア

	return;
};

/****************************************************************************/
/*   タスク定周期処理                                                       */
/* 　タスクスレッドで毎周期実行される関数			　                      */
/****************************************************************************/
void CClientService::routine_work(void* param) {
	input();
	main_proc();
	output();
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

UINT16 pb_ope_last[N_OTE_PNL_PB];

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
				CS_workbuf.auto_sel[i] = pOTE_IO->ote_umsg_in.body.auto_sel[i];
			}
//半自動はOTE管理にする
#if 0
			//半自動　PBL
			for (int i = ID_OTE_CHK_S1; i <= ID_OTE_CHK_N3; i++) {
				if (pOTE_IO->ote_umsg_in.body.pb_ope[i]) {
					CS_workbuf.semi_auto_selected = i;
					break;
				}
			}

			for (int i = ID_OTE_CHK_S1; i <= ID_OTE_CHK_N3; i++) {
				if (i == CS_workbuf.semi_auto_selected) {
					if (pOTE_IO->ote_umsg_in.body.pb_ope[i] > SEMI_AUTO_TG_RESET_TIME) {
						CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_ON;
						CS_workbuf.ote_pb_lamp[i].color = OTE0_MAZENDA;
					}
					else if (pOTE_IO->ote_umsg_in.body.pb_ope[i] > OTE0_PB_OFF_DELAY_COUNT) {
						CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_FLICK;
						CS_workbuf.ote_pb_lamp[i].color = OTE0_ORANGE;
					}
					else {
						CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_ON;
						CS_workbuf.ote_pb_lamp[i].color = OTE0_BLUE;
					}
				}
				else CS_workbuf.ote_pb_lamp[i].com = OTE_LAMP_COM_OFF;
			}
#endif
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

			//操作PB前回値保持
			for (int i = ID_OTE_PB_TEISHI; i < ID_OTE_CHK_N3; i++) {
				pb_ope_last[i] = pOTE_IO->ote_umsg_in.body.pb_ope[i];
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
		for (int i = ID_OTE_PB_TEISHI; i < ID_OTE_CHK_N3; i++) {
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

void CClientService::main_proc() {

	//操作端末処理

	ote_handle_proc();
	
	//＃＃＃ジョブイベント処理
	//半自動登録処理

	if (CS_workbuf.auto_mode == L_OFF) {
		//自動モードOFFでジョブホールド数クリア
		pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job = 0;
		CS_workbuf.semi_auto_selected = 0;
	}

	//イベント処理
	switch (CS_workbuf.job_set_event) {
	case CS_JOBSET_EVENT_SEMI_SEL_CLEAR: {
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];
		//		if (p_job->status == STAT_STANDBY) p_job->status = STAT_REQ_WAIT;
		p_job->status = STAT_REQ_WAIT;
		pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job--;
		if (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job < 0) pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;

		//イベントクリア
		CS_workbuf.job_set_event = CS_JOBSET_EVENT_CLEAR;
	}break;
	case CS_JOBSET_EVENT_SEMI_STANDBY: {										//半自動STANBY状態入り
		//*　半自動は、複数JOBの事前登録なし
				//現在のJOBバッファ
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];

		if (p_job->status != STAT_STANDBY) {//現在のJOB 起動待ちでない
			//次のバッファへ
			i_job = ++pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
			p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];
			
			if (i_job >= JOB_HOLD_MAX) {
				pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot = i_job = 0;
				p_job->status = STAT_ABOTED;
			}
			else {
				p_job->status = STAT_STANDBY;
			}

			//JOB LIST更新
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job++;	//減算は、POLICY呼び出しのupdate_job_status()の完了報告で実行
		}
		else {//現在のJOB既に起動待ち→ステータスホールド
			//現バッファを更新
			//i_job = ++CS_workbuf.job_list[ID_JOBTYPE_SEMI].i_job_hot;
			//p_job = &CS_workbuf.job_list[ID_JOBTYPE_SEMI].job[i_job];
			p_job->status = STAT_STANDBY;
		}

		 //JOB SET内容セット
		p_job->list_id = ID_JOBTYPE_SEMI;
		p_job->id = i_job;
		set_semi_recipe(p_job);							//JOBレシピセット
	
		//イベントクリア
		CS_workbuf.job_set_event = CS_JOBSET_EVENT_CLEAR;
	}break;
	case CS_JOBSET_EVENT_JOB_STANDBY: {							//JOB　STANDBY状態入り JOBはCLIENTからのJOB受信時にSTANDBY
		//*　JOBは、複数JOBの事前登録可能 
		if (pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job < JOB_REGIST_MAX - 1) {
			//バッファへの追加場所を評価
			int i_job = pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot + pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job;
			if (i_job > JOB_REGIST_MAX) i_job = i_job % JOB_REGIST_MAX;
			LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_JOB].job[i_job];


			//JOB LIST更新
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job++;	//減算は、POLICY呼び出しのupdate_job_status()の完了報告で実行

			//JOB SET内容セット
			p_job->status = STAT_STANDBY;
			p_job->list_id = ID_JOBTYPE_SEMI;
			p_job->id = i_job;
			set_job_recipe(p_job);							//JOBレシピセット

			CS_workbuf.job_set_event = CS_JOBSET_EVENT_CLEAR;
		}
		else {//登録制限数越えは、無視。警報表示必要
			CS_workbuf.job_set_event = CS_JOBSET_EVENT_JOB_OVERFLOW;
		}
	}break;
	case CS_JOBSET_EVENT_SEMI_TRIG: {							//半自動起動PBトリガあり
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];

		if (p_job->status == STAT_STANDBY) {//JOB起動待ち状態
			p_job->status = STAT_TRIGED;	//JOBトリガ状態にステータス更新(STANDBYでレシピ設定済　AGENT実行待ち
		}
	}break;
	case CS_JOBSET_EVENT_JOB_TRIG: {
		int i_job = pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot;
		LPST_JOB_SET p_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[i_job];

		if (p_job->status == STAT_STANDBY) {//JOB起動待ち状態
			p_job->status = STAT_TRIGED;	//JOBトリガ状態にステータス更新(STANDBYでレシピ設定済　AGENT実行待ち
		}
	}break;
	default:break;
	}

	//現在アクティブなJOB
	if ((pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job > 0) &&
		(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status != STAT_ACTIVE)){
		CS_workbuf.p_active_job = &pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot];
	}
	else if ((pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job > 0) &&
			 (pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status != STAT_ACTIVE)) {
		CS_workbuf.p_active_job = &pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot];
	}
	else
		CS_workbuf.p_active_job = NULL;


	return;
}

//ジョブのレシピセット
LPST_JOB_SET CClientService::set_job_recipe(LPST_JOB_SET pjob_set) {
	LPST_JOB_SET pjob = NULL;
	pjob_set->type = ID_JOBTYPE_JOB;
	return pjob;
}

//半自動のレシピセット
LPST_JOB_SET CClientService::set_semi_recipe(LPST_JOB_SET pjob_set) {
	//JOBのコマンド数　半自動は１
	pjob_set->n_com = 1;
	pjob_set->type = ID_JOBTYPE_SEMI;
	//目標位置セット
	pjob_set->recipe[0].target.pos[ID_HOIST] = CS_workbuf.semi_auto_selected_target.pos[ID_HOIST];
	pjob_set->recipe[0].target.pos[ID_BOOM_H] = CS_workbuf.semi_auto_selected_target.pos[ID_BOOM_H];
	pjob_set->recipe[0].target.pos[ID_SLEW] = CS_workbuf.semi_auto_selected_target.pos[ID_SLEW];

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
		wostrs << L" AS=" << CS_workbuf.antisway_mode << L",AUTO=" << CS_workbuf.auto_mode;

		int status;
		if (CS_workbuf.p_active_job != NULL) {
			status = CS_workbuf.p_active_job->status;
			wostrs << L",JOB TYPE=" << CS_workbuf.p_active_job->list_id << L", id=" << CS_workbuf.p_active_job->id;
		}
		else status = STAT_NA;

		if ((pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job <= 0) && (pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job <= 0)) {
			wostrs << L" >JOB: 0 hold";
		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_JOB) &&
			(pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job > 0)) {
			wostrs << L" >JOB: " << pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job << L" iHOT-> ";
			if (status & STAT_STANDBY)		wostrs << L"STANDBY";
			else if (status & STAT_ACTIVE)	wostrs << L"ACTIVE";
			else if (status & STAT_TRIGED)	wostrs << L"TRIGED";
			else if (status & STAT_SUSPENDED)	wostrs << L"SUSPEND";
			else if (status & STAT_REQ_WAIT)	wostrs << L"WAIT REQ";
			else wostrs << L"OUT OF SERV";

		}
		else if ((CS_workbuf.p_active_job != NULL) &&
			(CS_workbuf.p_active_job->list_id == ID_JOBTYPE_SEMI) &&
			(pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job > 0)) {
			wostrs << L" >SEMIAUTO: " << pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job << L" iHOT-> ";
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
//### AGENTからの実行待ちJOB問い合わせ応答
LPST_JOB_SET CClientService::get_next_job() {
	
	//HOT　JOBのステータスがトリガ状態のものを返信
	
	//半自動チェック
	int job_status = pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status;

	switch(job_status){
	case STAT_TRIGED:
		//レシピをセットしてポインタを返す
		 return &(pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot]);
	
	case STAT_ACTIVE:
	case STAT_SUSPENDED:
	case STAT_STANDBY:
	case STAT_ABOTED:
	case STAT_END:
	case STAT_REQ_WAIT:
		//実行待ち(TRIGGER）状態以外はスルー
		break;
	}

	//JOBチェック
	job_status = pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot].status;

	switch (job_status) {
	case STAT_TRIGED:
		//レシピをセットしてポインタを返す
		return &(pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_JOB].i_job_hot]);

	case STAT_ACTIVE:
	case STAT_SUSPENDED:
	case STAT_STANDBY:
	case STAT_ABOTED:
	case STAT_END:
	case STAT_REQ_WAIT:
		//実行待ち(TRIGGER）状態以外はスルー
		break;
	}
	//実行待ち無ければヌルリターン
	return NULL;
}

//### POLICYからのJOB Status更新依頼
int CClientService::update_job_status(LPST_JOB_SET pjobset, int fb_code) {

	if (pjobset->list_id == ID_JOBTYPE_JOB) {
		switch (fb_code) {
		case STAT_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
			break;
		}
		case STAT_ABNORMAL_END: {
			pJob_IO->job_list[ID_JOBTYPE_JOB].n_hold_job--;
			pJob_IO->job_list[ID_JOBTYPE_JOB].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
		}break;
		case STAT_ACTIVE :break;
		case STAT_ABOTED:break;
		default:break;
		}
	}
	else if (pjobset->list_id == ID_JOBTYPE_SEMI) {
		switch (fb_code) {
		case STAT_END: {
			//正常完了時JOBのホールド数を0クリア
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_END;

	//		CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;		//半自動設定クリア
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_CLEAR;	//目標確定解除
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_CLEAR;	//目標確定解除

			job_report2client(pjobset, STAT_END);

			return STAT_ACK;
		}break;
		case STAT_ABNORMAL_END: {
			//異常完了時JOBのホールド数を0クリア
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_ABNORMAL_END;

	//		CS_workbuf.semi_auto_selected = SEMI_AUTO_TG_CLR;		//半自動設定クリア
			CS_workbuf.target_set_z = CS_SEMIAUTO_TG_SEL_CLEAR;		//目標確定解除
			CS_workbuf.target_set_xy = CS_SEMIAUTO_TG_SEL_CLEAR;	//目標確定解除

			job_report2client(pjobset, STAT_ABNORMAL_END);

			return STAT_ACK;
		}break;
		case STAT_ACTIVE: {
			//ジョブ実行報告は、ステータスの更新のみ
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_ACTIVE;

			job_report2client(pjobset, STAT_ACTIVE);

			return STAT_ACK;
		}break;
		case STAT_ABOTED: {
			//ジョブキャンセル時（自動OFF等）JOBのホールド数を0クリア
			pJob_IO->job_list[ID_JOBTYPE_SEMI].n_hold_job = 0;
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_ABOTED;

			return STAT_ACK;
		}break;
		case STAT_SUSPENDED: {
			//中断時（手動介入）の場合は、ステータスをスタンバイ状態に戻す
			pJob_IO->job_list[ID_JOBTYPE_SEMI].job[pJob_IO->job_list[ID_JOBTYPE_SEMI].i_job_hot].status = STAT_STANDBY;

			return STAT_ACK;
		}break;
		default:return STAT_LOGICAL_ERROR;
		}
	}

	return STAT_LOGICAL_ERROR;
}

int CClientService::job_report2client(LPST_JOB_SET pjobset, int fb_code) {       //Jobの実行状況報告

	//後日検討　CLIENT通信アンサバック,ログ記録等
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
		}break;
		case STAT_ABNORMAL_END: {
		}break;
		case STAT_ACTIVE: {
		}break;
		case STAT_ABOTED: {
		}break;
		case STAT_SUSPENDED: {
		}break;
		default:return STAT_LOGICAL_ERROR;
		}
	}

	return STAT_SUCCEED;
	return STAT_NA;
}

/****************************************************************************/
/*   半自動関連処理															*/
/****************************************************************************/


/****************************************************************************/
/*   JOB関連																*/
/****************************************************************************/

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
