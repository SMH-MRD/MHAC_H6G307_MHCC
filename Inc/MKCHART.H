#pragma once


#include <Windows.h>
#include <commctrl.h>
#include <time.h>

#define NAME_OF_MK_INIFILE		L"mkchart"	//ini�t�@�C����
#define EXT_OF_MK_INIFILE		L"ini"		//ini�t�@�C���g���q

#define SET1_SECT_OF_INIFILE					L"SET1"			//CHART1 �@�\�p�����[�^�Z�N�V����
#define SET2_SECT_OF_INIFILE					L"SET2"			//CHART2 �@�\�p�����[�^�Z�N�V����
#define SET3_SECT_OF_INIFILE					L"SET3"			//CHART3 �@�\�p�����[�^�Z�N�V����
#define SET4_SECT_OF_INIFILE					L"SET4"			//CHART4 �@�\�p�����[�^�Z�N�V����

#define SET_CHART_TYPE_KEY_OF_INIFILE			L"CHART_TYPE"
#define SET_CHART_X_KEY_OF_INIFILE				L"CHART_WND_X"
#define SET_CHART_Y_KEY_OF_INIFILE				L"CHART_WND_Y"
#define SET_CHART_W_KEY_OF_INIFILE				L"CHART_WND_W"
#define SET_CHART_H_KEY_OF_INIFILE				L"CHART_WND_H"
#define SET_CHART_PLOT_MS_KEY_OF_INIFILE		L"PLOT_MS"

#define CHART_WND_X_DEF				20		//CHART WINDOW�ʒuDEFAULT
#define CHART_WND_Y_DEF				600		//CHART WINDOW�ʒuDEFAULT
#define CHART_WND_W_DEF				600		//CHART WINDOW��DEFAULT
#define CHART_WND_H_DEF				480		//CHART WINDOW����DEFAULT

#define CHART_WND_X_SCAT			620		//CHART WINDOW�ʒuSCATTER
#define CHART_WND_Y_SCAT			600		//CHART WINDOW�ʒuSCATTER
#define CHART_WND_W_SCAT			400		//CHART WINDOW��SCATTER
#define CHART_WND_H_SCAT			480		//CHART WINDOW����SCATTER

#define GRAPH_CHART_100				80		//100%�h�b�g��
#define GRAPH_CHART_DOT_H			200		//�P�̃`���[�g�̍���
#define GRAPH_CHART_DOT_W			500		//�P�̃`���[�g�̕�
#define CHART_MARGIN_X				50		//�`���[�g�����o���|�C���g
#define CHART_MARGIN_Y				15		//�`���[�g�����o���|�C���g
#define GRAPH_CHART_DISP_TIME_DEF	30000	//���ԃ`���[�g�\�����Ԕ͈͂̃f�t�H���gmsec
#define GRAPH_CHART_BOOL_H			4		//BOOL�f�[�^ON���̍���
#define GRAPH_CHART_PADDING_Y		20		//�w�i,�O���t�B�b�N�\���pY�����p�f�B���O�l

#define SCAT_CHART_100				80		//100%�h�b�g��
#define SCAT_CHART_DOT_H			200		//�P�̃`���[�g�̍���
#define SCAT_CHART_DOT_W			200		//�P�̃`���[�g�̕�

#define SCAT_NUM					2		//
#define SCAT_DURATION_DEF			10000	//�U�z�}�\���X�V����msec
#define SCAT_INTERVAL				100		//�U�z�}�\���Ԋu
#define SCAT_MARGIN_X				25		//�U�z�}�`���[�g�����o���|�C���g
#define SCAT_MARGIN_Y				10		//�U�z�}�`���[�g�����o���|�C���g

#define PHASE_MAX_DEGREE			20		//�U�z�}�`���[�g�\���ő�p�x



#define MK_CHART_WND_MAX			2		//�\���\�`���[�g�E�B���h�E��
#define MK_CHART_MAX_PER_WND		2		//1�E�B���h�E������\���\�`���[�g��
#define MK_MAX_GRAPH_PER_CHART		4		//1�̃`���[�g�ɏd�˂ĕ\���\�ȃO���t��
#define MK_MAX_BOOL_PER_CHART		4		//�P�̃O���t�ɏd�˂ĕ\���\��ON/OFF�f�[�^��

#define MK_WND_TYPE_TIME_GRAPH		0		//X:���ԁ@Y:DATA
#define MK_WND_TYPE_SCATTER			1		//�U�z�}
#define MK_CHART_WND1				MK_WND_TYPE_TIME_GRAPH
#define MK_CHART_WND2				MK_WND_TYPE_SCATTER

#define MK_CHART1					0
#define MK_CHART2					1
#define MK_CHART3					2
#define MK_CHART4					3
#define MK_GRAPH1					0
#define MK_GRAPH2					1
#define MK_GRAPH3					2
#define MK_GRAPH4					3

#define MK_DATA_TYPE_NULL			0x0000
#define MK_DATA_TYPE_TIME			0x0001
#define MK_DATA_TYPE_DOUBLE			0x0002
#define MK_DATA_TYPE_INT			0x0004
#define MK_DATA_TYPE_BOOL			0x0008
#define MK_DATA_TYPE_ANALOG			0x0006

#define MK_CHART_BUF_MAX			600
#define MK_RANGE_100PERCENT			100000	//1%->1000 = 0.001%�P��

#define MK_CHART_REFRESH_MS			10000
#define MK_CHART_TIME_100			10000

#define MK_CHART_PLOT_AXIS			2
#define MK_DATA_CODE_X				0
#define MK_DATA_CODE_Y				1
#define MK_ADDRESS_INT				0
#define MK_ADDRESS_DOUBLE			0
#define MK_ADDRESS_BOOL_MAX			4
#define MK_ADDRESS_BOOL1			0
#define MK_ADDRESS_BOOL2			1
#define MK_ADDRESS_BOOL3			2
#define MK_ADDRESS_BOOL4			3

//����{�^��ID
#define IDC_CHART_START_PB			10601
#define IDC_CHART_PAUSE_PB			10602
#define IDC_CHART_RADIO_OPT1		10603
#define IDC_CHART_RADIO_OPT2		10604

//�N���^�C�}�[ID
#define ID_CHART_TIMER				100		//100msec

//���b�Z�[�WID
#define WM_MKCHART_INIT				0x0400



	union MK_plot {					//���ԃ`���[�g�pY���v���b�g�l�̗v�f�i�A�i���O�܂���BIT�j�`���[�g�Ƀv���b�g����f�[�^�@ON/OFF��4�_
		INT32 i_data;
		bool b_data[4];
	};

	typedef struct _stMK_plot {		//���ԃ`���[�g�pX,Y���v���b�g�l�̗v�f
		INT32 x;
		MK_plot y;		//�A�i���O100%(0.001%�P�ʁj�Ő��K���@�f�W�^����bool�@
	}ST_PLOT_ELEMENT, * LPST_PLOT_ELEMENT;

	typedef struct _stPLOT_BUF {	//�e�O���t�̎��ԃ`���[�g�pX,Y���v���b�g�l�̌��f�[�^�̃|�C���^�̃|�C���^
		double** ppd[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART];
		int** ppi[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART];
		bool** ppb[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART][MK_MAX_BOOL_PER_CHART];
	}ST_PLOT_BUF, * LPST_PLOT_BUF;


	typedef struct _stRANGE_SET		//�e�O���t�̎��ԃ`���[�g�pX,Y���v���b�g�l�̌��f�[�^100%�l
	{
		double** ppd100[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART];
		int**    ppi100[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART];
		bool**   ppb100[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART][MK_MAX_BOOL_PER_CHART];
	}ST_RANGE_SET, * LPST_RANGE_SET;

//chart_status��`
#define MK_CHART_OPEN				0x0001	//�\����
#define MK_CHART_ACTIVE				0x0002	//�`��X�V��
#define MK_CHART_PAUSE				0x0004	//�`��X�V�ۗ���
#define MK_CHART_NO_LINE			0x0100	//����
#define MK_CHART_NO_MARK			0x0200	//�}�[�J��

//chart_status��`�f�t�H���g�l
#define	MK_CHART_STATUS_DEF_BASIC	0x0202	//�}�[�J���@�`��X�V��
#define	MK_CHART_STATUS_DEF_SCATTER 0x0102	//�����@�@�@�`��X�V��

//chart_type��`
//#define MK_CHART_TIME_GRAPH			0x0001
//#define MK_CHART_SCATTER			0x0002

//�v���b�g�ʒu�i�[�z��C���f�b�N�X
#define MK_CHART_AO						0
#define MK_CHART_DO_0					0
#define MK_CHART_DO_1					1
#define MK_CHART_DO_2					2
#define MK_CHART_DO_3					3

	typedef struct _stCHART_SET
	{
		WORD chart_status;						//HBYTE �\���ݒ�	LBYTE ���s���
		WORD chart_type;						//�O���t�^�C�v
		int num_of_plot;						//�v���b�g��
		int chart_x, chart_y, chart_w, chart_h;	//�`���[�g��ʂ̈ʒu�@���A����
		int graph_field_w, graph_field_h;		//Time GRAPH�`���[�g�\���̈�̃r�b�g�}�b�v�T�C�Y
		int plot_interval_ms=100;				//�\���Ԋumsec�i�^�C�}�[�C�x���g�̐ݒ�l�j
		int refresh_interval=100;				//�O���t�\���N���A�Ԋu(�v���b�g��	�U�z�}�p�j
		DWORD	start_time_ms;					//�`���[�g�J�n���ԁiPC���N�����Ă���̋N�����ԁj
		DWORD	latast_plot_time_ms;			//�Ō�Ƀv���b�g�������ԁiPC���N�����Ă���̋N�����ԁj

	//Window
		HWND hwnd_chart;						//�`���[�g�E�B���h�E�̃n���h��
		HWND hwnd_chart_startPB;				//�`���[�g�X�^�[�gPB�̃n���h��
		HWND hwnd_chart_pausePB;				//�`���[�g�X�g�b�vPB�̃n���h��
		HWND hwnd_chart_opt1_radio;				//�`���[�gOption1PB�̃n���h��
		HWND hwnd_chart_opt2_radio;				//�`���[�gOption2PB�̃n���h��
		
		HBITMAP hBmap_mem0;						//�O���t�`��r�b�g�}�b�v�������i�S�́j
		HBITMAP hBmap_bg;						//�O���t�`��r�b�g�}�b�v�������i�w�i�j
		HBITMAP hBmap_graph;					//�O���t�`��r�b�g�}�b�v�������i�O���t�`�敔�j
		HBITMAP hBmap_inf;						//�O���t�`��r�b�g�}�b�v�������i�e�L�X�g�\�����j

		HDC hdc_mem0;							//������ʃ������f�o�C�X�R���e�L�X�g
		HDC hdc_mem_bg;							//�w�i��ʃ������f�o�C�X�R���e�L�X�g
		HDC hdc_mem_graph;						//�O���t��ʃ������f�o�C�X�R���e�L�X�g
		HDC hdc_mem_inf;						//������ʃ������f�o�C�X�R���e�L�X�g

		HFONT hfont_inftext;					//�e�L�X�g�p�t�H���g
		BLENDFUNCTION bf;						//�����ߐݒ�\����

		int g_ms_per_dot;						//1dot������̎���(msec)
		int dot100percent_x, dot100percent_y;	//100%�Ɋ��蓖�Ă�dot��
		POINT g_origin[MK_MAX_GRAPH_PER_CHART];	//�O���t�̌��_

		bool bPrimaryDisp;						//���ԃ`���[�g��2�摜�؂�ւ�

		//���f�[�^�֘A		
		ST_PLOT_BUF pdata[MK_CHART_PLOT_AXIS];					//�e�O���t�̎��ԃ`���[�g�p�v���b�g�l�̌��f�[�^�̃|�C���^���Z�b�g�����\����
		ST_RANGE_SET pvalue100[MK_CHART_PLOT_AXIS];				//�v���b�g�f�[�^100%�̒l�̃|�C���^���Z�b�g�����\���́@���Ԃ́A100�b=100%�Ƃ��� 0�Z�b�g�Ńv���b�g���Ȃ�
		int	data_type[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART][MK_CHART_PLOT_AXIS];	//�v���b�g�f�[�^�̌^�@[WND�������CHART��][CHART������̃O���t��][x��y��]

		UINT_PTR timerID;//�v���b�g�^�C�}�[ID

		int plot_buf_index;				//plot_data�̏�������index
		int spd_dot_per_sec;			//plot���x
		int graph_count[MK_CHART_MAX_PER_WND];
	
		//�v���b�g�l�֘A
		ST_PLOT_ELEMENT	plot_data[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART][MK_CHART_BUF_MAX];	//�O�����v���b�g�l(���K���l�j
		POINT plot_p[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART][MK_MAX_BOOL_PER_CHART];			//�r�b�g�}�b�v��̃v���b�g�_

		HPEN hpen[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART];		//�O���t�`��p��PEN�n���h��
		HBRUSH hbrush[MK_CHART_MAX_PER_WND][MK_MAX_GRAPH_PER_CHART];	//�O���t�`��p��BRUSH�n���h��

	}ST_CHART_SET, * LPST_CHART_SET;

#define MK_CHART_N_HPEN		20
#define MK_CHART_N_HBRUSH	20

#define MK_CHART_NULL		0
#define MK_CHART_BLACK		1
#define MK_CHART_WHITE		2
#define MK_CHART_RED		3
#define MK_CHART_BLUE		4
#define MK_CHART_GREEN		5
#define MK_CHART_YELLOW		6
#define MK_CHART_MAGENDA	7
#define MK_CHART_LIGHT_BLUE	8
#define MK_CHART_GLAY		9


	class CMKChart
	{
	private:
		CMKChart();		//static�N���X�ɂ���̂�private�ɂ���
		~CMKChart();	//static�N���X�ɂ���̂�private�ɂ���
	public:

		static WCHAR szInipath[_MAX_PATH], szDrive[_MAX_DRIVE], szPath[_MAX_PATH], szFName[_MAX_FNAME], szExt[_MAX_EXT];//ini�t�@�C���p�X�擾�p
		static WCHAR szLogfolder1[_MAX_PATH];				//�`���[�g���O�p�t�H���_�p�X�擾�p
		static std::wstring log_path;						//�`���[�g���O�p�t�H���_�p�X�擾�p
		
		static ST_CHART_SET mkchartset[MK_CHART_WND_MAX];	//�`���[�g�Ǘ��\����
		static HINSTANCE hInst;
		static HPEN hpens[MK_CHART_N_HPEN];
		static HBRUSH hbrushes[MK_CHART_N_HBRUSH];
		static int init_chartfunc();
		static int init_chart(int chartID);
		static HWND open_chart(int chartID, HWND hwnd_parent);
		static int clear_chart(int chartID);

		static int set_data_type(int type, int chart_WND_ID, int i_chart, int i_item, bool is_x);
		static int set_double_data(double** ppd, int chart_WND_ID, int i_chart, int i_item, double** ppd_100, bool is_x);
		static int set_int_data(int** ppi, int chart_WND_ID, int i_chart, int i_item, int** ppi_100, bool is_x);
		static int set_bool_data(bool** ppb, int chart_WND_ID, int i_chart, int i_item, int i_bool, bool** ppb100);
		static int clear_set_data(int iwnd);
		
		static int set_chart_spd(int chart_WND_ID, int disp_time_ms);//�`���[�g�̃X�s�[�h�@�`���[�g���L�^�ɂ����鎞��msec
		static int set_reflesh_time(int chart_WND_ID, int period_ms);

		static bool is_chart_open(int iwnd) {if (mkchartset[iwnd].hwnd_chart == NULL)return false;else return true;	}
		static int chart_start(int chartID, HWND hwnd_parent);
		static int get_chartID(HWND hWnd) { for (int i = 0; i < MK_CHART_WND_MAX; i++) { if (hWnd == mkchartset[i].hwnd_chart) return i; } return 0; }
		static int set_graph(int chart_WND_ID);
		static void draw_bg(int chartID);

		static LRESULT CALLBACK ChartWndProc_A(HWND, UINT, WPARAM, LPARAM);//Time-Data Graph
		static LRESULT CALLBACK ChartWndProc_B(HWND, UINT, WPARAM, LPARAM);//Data-Data Graph
	};