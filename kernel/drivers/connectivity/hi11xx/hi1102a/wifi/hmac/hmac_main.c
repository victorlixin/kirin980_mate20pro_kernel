


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "oal_ext_if.h"
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
#include "oal_profiling.h"
#endif

#if defined(_PRE_FEATURE_PLAT_LOCK_CPUFREQ) && !defined(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/hisi/hisi_cpufreq_req.h>
#include <linux/cpufreq.h>
#include <linux/hisi/hisi_core_ctl.h>
#include <linux/pm_qos.h>
#endif
#endif

#include "oam_ext_if.h"
#include "oam_trace.h"
#include "frw_ext_if.h"
#include "mac_device.h"
#include "mac_resource.h"
#include "mac_regdomain.h"
#include "mac_board.h"

#include "dmac_ext_if.h"

#include "hmac_fsm.h"
#include "hmac_main.h"
#include "hmac_vap.h"
#include "hmac_tx_amsdu.h"
#include "hmac_rx_data.h"
#include "hmac_11i.h"
#include "hmac_mgmt_classifier.h"

#include "hmac_chan_mgmt.h"
#include "hmac_dfs.h"
#ifdef _PRE_WLAN_CHIP_TEST
#include "hmac_test_main.h"
#endif

#include "hmac_data_acq.h"
#include "hmac_rx_filter.h"

#include "hmac_hcc_adapt.h"

#include "hmac_dfs.h"
#include "hmac_config.h"
#include "hmac_resource.h"
#include "hmac_device.h"
#include "hmac_scan.h"
#include "hmac_hcc_adapt.h"
#include "hmac_dfx.h"
#if defined(_PRE_PRODUCT_ID_HI110X_HOST)
#include "hmac_cali_mgmt.h"
#endif
#ifdef _PRE_SUPPORT_ACS
#include "dmac_acs.h"
#include "hmac_acs.h"
#endif
#ifdef _PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT
#include "oal_kernel_file.h"
#endif

#include "oal_hcc_host_if.h"
#include "oal_net.h"
#include "hmac_tcp_opt.h"
#include "hmac_device.h"
#include "hmac_vap.h"
#include "hmac_resource.h"
#include "hmac_mgmt_classifier.h"
#ifdef _PRE_WLAN_FEATURE_AUTO_FREQ
#include "hmac_ext_if.h"
#endif
#include "hmac_statistic_data_flow.h"

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
#include "hmac_proxysta.h"
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm_wlan.h"
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102A_HOST)
#include "hmac_auto_ddr_freq.h"
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_MAIN_C

/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/


/* hmacģ����ӵ�ȫ�ֿ��Ʊ��� */
mac_board_stru g_st_hmac_board;

oal_wakelock_stru g_st_hmac_wakelock;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
hmac_rxdata_thread_stru     g_st_rxdata_thread;
#endif
/* ��Ƶ�ʽӿ� */
#if defined(_PRE_FEATURE_PLAT_LOCK_CPUFREQ) && !defined(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)
struct cpufreq_req g_ast_cpufreq[OAL_BUS_MAXCPU_NUM];
struct pm_qos_request g_st_pmqos_requset;
oal_uint32 g_aul_cpumaxfreq[OAL_BUS_MAXCPU_NUM];
#endif
OAL_STATIC oal_uint32  hmac_create_ba_event(frw_event_mem_stru  *pst_event_mem);
OAL_STATIC oal_uint32  hmac_del_ba_event(frw_event_mem_stru  *pst_event_mem);
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
OAL_STATIC oal_uint32  hmac_syn_info_event(frw_event_mem_stru  *pst_event_mem);
OAL_STATIC oal_uint32  hmac_voice_aggr_event(frw_event_mem_stru  *pst_event_mem);
#endif

oal_uint32  hmac_init_scan_do(hmac_device_stru *pst_mac_dev, mac_vap_stru *pst_vap, mac_init_scan_req_stru *pst_cmd);

#ifdef _PRE_SUPPORT_ACS
oal_uint32  hmac_event_acs_response(frw_event_mem_stru  *pst_event_mem);
oal_uint32  hmac_config_set_acs_cmd(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param);
oal_uint32  hmac_acs_got_init_rank(hmac_device_stru *pst_hmac_dev, mac_vap_stru *pst_mac_vap, mac_acs_cmd_stru *pst_cmd);
#endif

#ifdef _PRE_WLAN_FEATURE_FLOWCTL
OAL_STATIC oal_uint32  hmac_flowctl_backp_event(frw_event_mem_stru  *pst_event_mem);
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && defined(_PRE_WLAN_CHIP_TEST_ALG)
extern  oal_uint32  hmac_alg_test_result_process(frw_event_mem_stru  *pst_event_mem);
extern  oal_int32   hmac_alg_test_main_common_init(oal_void);
extern  oal_int32   hmac_alg_test_main_common_exit(oal_void);
#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
extern oal_int32 hmac_hcc_adapt_init(oal_void);
extern oal_uint32 hmac_hcc_adapt_deinit(oal_void);
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
oal_uint32 hmac_btcoex_check_by_ba_size(hmac_user_stru *pst_hmac_user);
#endif
/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

oal_void  hmac_board_get_instance(mac_board_stru **ppst_hmac_board)
{
    *ppst_hmac_board = &g_st_hmac_board;
}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

 oal_uint32 hmac_init_event_process(frw_event_mem_stru *pst_event_mem)
{
    frw_event_stru        *pst_event;             /* �¼��ṹ�� */
    mac_data_rate_stru    *pst_data_rate;
    dmac_tx_event_stru    *pst_ctx_event;
    mac_device_stru       *pst_mac_device;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_init_event_process::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = (frw_event_stru *)pst_event_mem->puc_data;

    pst_ctx_event = (dmac_tx_event_stru *)pst_event->auc_event_data;
    pst_data_rate = (mac_data_rate_stru *)(oal_netbuf_data(pst_ctx_event->pst_netbuf));

    /* ͬ��mac֧�ֵ����ʼ���Ϣ */
    pst_mac_device = mac_res_get_dev(pst_event->st_event_hdr.uc_device_id);
    if (OAL_PTR_NULL == pst_mac_device)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_init_event_process::pst_mac_device null.}");
        oal_netbuf_free(pst_ctx_event->pst_netbuf);
        return OAL_ERR_CODE_MAC_DEVICE_NULL;
    }

    oal_memcopy((oal_uint8 *)(pst_mac_device->st_mac_rates_11g),
        (oal_uint8 *)pst_data_rate,
        sizeof(mac_data_rate_stru) * MAC_DATARATES_PHY_80211G_NUM);

    /* �ͷŵ�02ͬ����Ϣ���õ�netbuf��Ϣ */
    oal_netbuf_free(pst_ctx_event->pst_netbuf);

    return OAL_SUCC;
}
#else

 oal_uint32 hmac_init_event_process(frw_event_mem_stru *pst_event_mem)
{
    frw_event_stru        *pst_event;             /* �¼��ṹ�� */
    mac_chip_stru         *pst_chip;
    oal_uint32             ul_return;
    oal_uint32             ul_chip_max_num;

    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_init_event_process::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = (frw_event_stru *)pst_event_mem->puc_data;
    pst_chip = (mac_chip_stru *)(pst_event->auc_event_data);


    /* chip֧�ֵ��������PCIe���ߴ����ṩ; */
    ul_chip_max_num = oal_bus_get_chip_num();

    if (OAL_UNLIKELY(ul_chip_max_num > WLAN_CHIP_MAX_NUM_PER_BOARD))
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hmac_init_event_process::invalid ul_chip_max_num[%d].}", ul_chip_max_num);
        return OAL_ERR_CODE_ARRAY_OVERFLOW;
    }

    ul_return = hmac_board_init(ul_chip_max_num, pst_chip);
    if (OAL_SUCC != ul_return)
    {
         return OAL_FAIL;
    }

    return OAL_SUCC;
}


#endif
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_void hmac_event_fsm_tx_adapt_subtable_register(oal_void)
{
    /* ע��WLAN_CTX�¼����������� */
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_ACTION].p_tx_adapt_func    = hmac_proc_tx_process_action_event_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_ADD_USER].p_tx_adapt_func  = hmac_proc_add_user_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_DEL_USER].p_tx_adapt_func  = hmac_proc_del_user_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_MGMT].p_tx_adapt_func      = hmac_proc_mgmt_ctx_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_STA_SET_EDCA_REG].p_tx_adapt_func = hmac_proc_set_edca_param_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCAN_REQ].p_tx_adapt_func = hmac_scan_proc_scan_req_event_tx_adapt;
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
        dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_DPD_DATA_PROCESSED].p_tx_adapt_func = hmac_dpd_data_processed_event_tx_adapt;
#endif
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_CALI_HMAC2DMAC].p_tx_adapt_func = hmac_send_event_netbuf_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_CALI_MATRIX_HMAC2DMAC].p_tx_adapt_func  = hmac_send_event_netbuf_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_APP_IE_H2D].p_tx_adapt_func     = hmac_send_event_netbuf_tx_adapt;
#ifdef _PRE_WLAN_FEATURE_IP_FILTER
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_IP_FILTER].p_tx_adapt_func = hmac_config_update_ip_filter_tx_adapt;
#endif //_PRE_WLAN_FEATURE_IP_FILTER
#ifdef _PRE_WLAN_FEATURE_APF
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_APF].p_tx_adapt_func = hmac_send_event_netbuf_tx_adapt;
#endif

    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_SCHED_SCAN_REQ].p_tx_adapt_func = hmac_scan_proc_sched_scan_req_event_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_ASOC_WRITE_REG].p_tx_adapt_func = hmac_mgmt_update_user_qos_table_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_SET_REG].p_tx_adapt_func      = hmac_proc_join_set_reg_event_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_JOIN_DTIM_TSF_REG].p_tx_adapt_func    = hmac_proc_join_set_dtim_reg_event_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_CONN_RESULT].p_tx_adapt_func    = hmac_hcc_tx_convert_event_to_netbuf_uint32;
#ifdef _PRE_WLAN_FEATURE_OPMODE_NOTIFY
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_PSM_OPMODE_NOTIFY].p_tx_adapt_func  = hmac_hcc_tx_convert_event_to_netbuf_uint16;
#endif
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_SET_RX_FILTER].p_tx_adapt_func  = hmac_hcc_tx_convert_event_to_netbuf_uint32;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_NOTIFY_ALG_ADD_USER].p_tx_adapt_func  = hmac_user_add_notify_alg_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_BA_SYNC].p_tx_adapt_func           = hmac_proc_rx_process_sync_event_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WALN_CTX_EVENT_SUB_TYPR_SELECT_CHAN].p_tx_adapt_func    = hmac_chan_select_channel_mac_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_SWITCH_TO_NEW_CHAN].p_tx_adapt_func = hmac_chan_initiate_switch_to_new_channel_tx_adapt;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPR_RESTART_NETWORK].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
    dmac_wlan_ctx_event_sub_table[DMAC_WALN_CTX_EVENT_SUB_TYPR_DISABLE_TX].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
    dmac_wlan_ctx_event_sub_table[DMAC_WALN_CTX_EVENT_SUB_TYPR_ENABLE_TX].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;

#ifdef _PRE_WLAN_FEATURE_EDCA_OPT_AP
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPR_EDCA_OPT].p_tx_adapt_func  = hmac_edca_opt_stat_event_tx_adapt;
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
#ifdef _PRE_WLAN_FEATURE_OFFCHAN_CAC
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPR_SWITCH_TO_OFF_CHAN].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPR_SWITCH_TO_HOME_CHAN].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
#endif
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPR_DFS_TEST].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
#endif
    dmac_wlan_ctx_event_sub_table[DMAC_WLAN_CTX_EVENT_SUB_TYPE_RESET_PSM].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;

    /* ע��HOST_DRX�¼����������� */
    dmac_tx_host_drx[DMAC_TX_HOST_DRX].p_tx_adapt_func = hmac_proc_tx_host_tx_adapt;

    /* ע��HOST_CRX�¼����������� */
    dmac_host_crx_table[HMAC_TO_DMAC_SYN_INIT].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
    dmac_host_crx_table[HMAC_TO_DMAC_SYN_CREATE_CFG_VAP].p_tx_adapt_func = hmac_hcc_tx_convert_event_to_netbuf_uint16;
    dmac_host_crx_table[HMAC_TO_DMAC_SYN_CFG].p_tx_adapt_func = hmac_proc_config_syn_tx_adapt;
    dmac_host_crx_table[HMAC_TO_DMAC_SYN_ALG].p_tx_adapt_func = hmac_proc_config_syn_alg_tx_adapt;
#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE)
    dmac_host_crx_table[HMAC_TO_DMAC_SYN_SAMPLE].p_tx_adapt_func = hmac_sdt_recv_sample_cmd_tx_adapt;
#endif
    /* liuming add support keep alive code for 1102 */
    /* ע��WLAN_DTX�¼����������� */
}



OAL_STATIC oal_void hmac_event_fsm_rx_adapt_subtable_register(oal_void)
{
    frw_event_sub_rx_adapt_table_init(g_ast_hmac_wlan_drx_event_sub_table,
                                        OAL_SIZEOF(g_ast_hmac_wlan_drx_event_sub_table)/OAL_SIZEOF(frw_event_sub_table_item_stru),
                                        hmac_hcc_rx_convert_netbuf_to_event_default);

    frw_event_sub_rx_adapt_table_init(g_ast_hmac_wlan_crx_event_sub_table,
                                        OAL_SIZEOF(g_ast_hmac_wlan_crx_event_sub_table)/OAL_SIZEOF(frw_event_sub_table_item_stru),
                                        hmac_hcc_rx_convert_netbuf_to_event_default);

    frw_event_sub_rx_adapt_table_init(g_ast_hmac_wlan_ctx_event_sub_table,
                                        OAL_SIZEOF(g_ast_hmac_wlan_ctx_event_sub_table)/OAL_SIZEOF(frw_event_sub_table_item_stru),
                                        hmac_hcc_rx_convert_netbuf_to_event_default);

    frw_event_sub_rx_adapt_table_init(g_ast_hmac_tbtt_event_sub_table,
                                        OAL_SIZEOF(g_ast_hmac_tbtt_event_sub_table)/OAL_SIZEOF(frw_event_sub_table_item_stru),
                                        hmac_hcc_rx_convert_netbuf_to_event_default);

    frw_event_sub_rx_adapt_table_init(g_ast_hmac_wlan_misc_event_sub_table,
                                        OAL_SIZEOF(g_ast_hmac_wlan_misc_event_sub_table)/OAL_SIZEOF(frw_event_sub_table_item_stru),
                                        hmac_hcc_rx_convert_netbuf_to_event_default);

    g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_STA].p_rx_adapt_func = hmac_rx_process_data_sta_rx_adapt;
    g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_AP].p_rx_adapt_func  = hmac_rx_process_data_sta_rx_adapt;

    /* ע��HMACģ��WLAN_CRX�¼��ӱ� */
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_INIT].p_rx_adapt_func = hmac_rx_convert_netbuf_to_netbuf_default;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_RX].p_rx_adapt_func = hmac_rx_process_mgmt_event_rx_adapt;
#ifdef _PRE_WLAN_FEATURE_FLOWCTL
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_FLOWCTL_BACKP].p_rx_adapt_func = hmac_alg_flowctl_backp_rx_adapt;
#endif

    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_EVERY_SCAN_RESULT].p_rx_adapt_func = hmac_rx_convert_netbuf_to_netbuf_default;
#ifdef  _PRE_WLAN_RF_110X_CALI_DPD
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_TO_HMAC_DPD_CALIBRATED_DATA_SEND].p_rx_adapt_func = hmac_rx_convert_netbuf_to_netbuf_default;
#endif
#ifdef _PRE_WLAN_FEATURE_APF
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_APF_REPORT].p_rx_adapt_func = hmac_apf_program_report_rx_adapt;
#endif

    /* ע��MISC�¼��ӱ� */
	g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_CALI_TO_HMAC].p_rx_adapt_func  = hmac_cali2hmac_misc_event_rx_adapt;

}

#endif


OAL_STATIC oal_void hmac_event_fsm_action_subtable_register(oal_void)
{
    /* ���¼����ͺ͵��ú���������ע�ᵽ�¼�����ģ�� */
    /* ע��WLAN_DTX�¼��ӱ� */
    g_ast_hmac_wlan_dtx_event_sub_table[DMAC_TX_WLAN_DTX].p_func = hmac_tx_wlan_to_wlan_ap;

    /* APģʽ��ע��HMACģ��WLAN_DRX�¼��ӱ� */
    g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_AP].p_func = hmac_rx_process_data_ap;

    /* STAģʽ��ע��HMACģ��WLAN_DRX�¼��ӱ� */
    g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_RX_STA].p_func = hmac_rx_process_data_sta;

    /* AP ��STA ������ע��HMACģ��WLAN_DRX�¼��ӱ� */
    g_ast_hmac_wlan_drx_event_sub_table[DMAC_WLAN_DRX_EVENT_SUB_TYPE_TKIP_MIC_FAILE].p_func = hmac_rx_tkip_mic_failure_process;

    /* ע��HMACģ��WLAN_CRX�¼��ӱ� */
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_INIT].p_func = hmac_init_event_process;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_RX].p_func = hmac_rx_process_mgmt_event;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_DELBA].p_func = hmac_mgmt_rx_delba_event;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_EVERY_SCAN_RESULT].p_func = hmac_scan_proc_scanned_bss;
#ifdef _PRE_WLAN_RF_110X_CALI_DPD
      g_ast_hmac_wlan_crx_event_sub_table[DMAC_TO_HMAC_DPD_CALIBRATED_DATA_SEND].p_func = hmac_dpd_cali_data_recv;
#endif
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_SCAN_COMP].p_func = hmac_scan_proc_scan_comp_event;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_CHAN_RESULT].p_func = hmac_scan_process_chan_result_event;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_DISASSOC].p_func = hmac_mgmt_send_disasoc_deauth_event;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_DEAUTH].p_func = hmac_mgmt_send_disasoc_deauth_event;

#ifdef _PRE_SUPPORT_ACS
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_ACS_RESP].p_func = hmac_event_acs_response;
#endif

#ifdef _PRE_WLAN_FEATURE_FLOWCTL
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPE_FLOWCTL_BACKP].p_func = hmac_flowctl_backp_event;
#endif

    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPR_CH_SWITCH_COMPLETE].p_func = hmac_chan_switch_to_new_chan_complete;
    g_ast_hmac_wlan_crx_event_sub_table[DMAC_WLAN_CRX_EVENT_SUB_TYPR_DBAC].p_func = hmac_dbac_status_notify;

    /* ע��TBTT�¼��ӱ� */
    g_ast_hmac_tbtt_event_sub_table[DMAC_TBTT_EVENT_SUB_TYPE].p_func = hmac_mgmt_tbtt_event;

    /* ע�ᷢ��HOST��������¼��ӱ� */
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_SYN_UP_REG_VAL].p_func = hmac_sdt_up_reg_val;
#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE)
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_SYN_UP_SAMPLE_DATA].p_func = hmac_sdt_up_sample_data;
#endif
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_CREATE_BA].p_func      = hmac_create_ba_event;
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_DEL_BA].p_func         = hmac_del_ba_event;
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_SYN_CFG].p_func        = hmac_event_config_syn;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) && defined(_PRE_WLAN_CHIP_TEST_ALG)
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_ALG_TEST].p_func       = hmac_alg_test_result_process;
#endif

#ifdef _PRE_WLAN_FEATURE_DFS
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_RADAR_DETECT].p_func          = hmac_dfs_radar_detect_event;

#endif   /* end of _PRE_WLAN_FEATURE_DFS */
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_DISASOC].p_func               = hmac_proc_disasoc_misc_event;

#ifdef _PRE_SUPPORT_ACS
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_RESCAN].p_func                = hmac_acs_process_rescan_event;
#endif //_PRE_WLAN_FEATURE_ROAM
#ifdef _PRE_WLAN_FEATURE_APF
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_APF_REPORT].p_func = hmac_apf_program_report_event;
#endif

#ifdef _PRE_WLAN_FEATURE_ROAM
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_ROAM_TRIGGER].p_func          = hmac_proc_roam_trigger_event;
#endif //_PRE_WLAN_FEATURE_ROAM


#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    g_ast_hmac_host_drx_event_sub_table[HMAC_TX_HOST_DRX].p_func = hmac_tx_event_process;
    frw_event_table_register(FRW_EVENT_TYPE_HOST_DRX, FRW_EVENT_PIPELINE_STAGE_0, g_ast_hmac_host_drx_event_sub_table);
#endif

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_ALG_INFO_SYN].p_func           = hmac_syn_info_event;
    g_ast_hmac_wlan_ctx_event_sub_table[DMAC_TO_HMAC_VOICE_AGGR].p_func             = hmac_voice_aggr_event;

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    g_ast_hmac_wlan_misc_event_sub_table[DMAC_MISC_SUB_TYPE_CALI_TO_HMAC].p_func          = hmac_save_cali_event;
#endif
#endif
}


oal_uint32  hmac_event_fsm_register(oal_void)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

    /*ע�������¼���tx adapt�ӱ�*/
    hmac_event_fsm_tx_adapt_subtable_register();

    /*ע�������¼���rx adapt�ӱ�*/
    hmac_event_fsm_rx_adapt_subtable_register();
#endif

    /*ע�������¼���ִ�к����ӱ�*/
    hmac_event_fsm_action_subtable_register();

    event_fsm_table_register();

    return OAL_SUCC;
}


oal_int32 hmac_param_check(oal_void)
{
    /*netbuf's cb size!*/
    oal_uint32 ul_netbuf_cb_size = (oal_uint32)OAL_NETBUF_CB_SIZE();
    if(ul_netbuf_cb_size < (oal_uint32)OAL_SIZEOF(mac_tx_ctl_stru))
    {
        OAL_IO_PRINT("mac_tx_ctl_stru size[%u] large then netbuf cb max size[%u]\n",
                        ul_netbuf_cb_size,(oal_uint32)OAL_SIZEOF(mac_tx_ctl_stru));
        return OAL_EFAIL;
    }

    if(ul_netbuf_cb_size < (oal_uint32)OAL_SIZEOF(mac_rx_ctl_stru))
    {
        OAL_IO_PRINT("mac_rx_ctl_stru size[%u] large then netbuf cb max size[%u]\n",
                        ul_netbuf_cb_size,(oal_uint32)OAL_SIZEOF(mac_rx_ctl_stru));
        return OAL_EFAIL;
    }
    return OAL_SUCC;
}

#if defined(_PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
/*debug sysfs*/
extern oal_int32 hmac_tx_event_pkts_info_print(oal_void* data, char* buf, oal_int32 buf_len);
OAL_STATIC oal_kobject* g_conn_syfs_hmac_object = NULL;
OAL_STATIC oal_int32 hmac_print_vap_stat(oal_void* data, char* buf, oal_int32 buf_len)
{
    oal_int32 ret = 0;
    oal_uint8               uc_vap_id;
    oal_net_device_stru    *pst_net_device = NULL;
    hmac_vap_stru          *pst_hmac_vap = NULL;
    for (uc_vap_id = 0; uc_vap_id < WLAN_VAP_MAX_NUM_PER_DEVICE_LIMIT; uc_vap_id++)
    {
        pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(uc_vap_id);
        if(NULL == pst_hmac_vap)
        {
            continue;
        }
        ret +=  OAL_SPRINTF(buf + ret , buf_len - ret, "vap %2u info:\n",uc_vap_id);
        ret +=  OAL_SPRINTF(buf + ret , buf_len - ret, "vap_state %2u,protocol:%2u,user nums:%2u,init:%u\n",
                                                    pst_hmac_vap->st_vap_base_info.en_vap_state,
                                                    pst_hmac_vap->st_vap_base_info.en_protocol,
                                                    pst_hmac_vap->st_vap_base_info.us_user_nums,
                                                    pst_hmac_vap->st_vap_base_info.uc_init_flag);
        pst_net_device = pst_hmac_vap->pst_net_device;
        if(NULL != pst_net_device)
        {
            oal_int32 i;
            ret +=  OAL_SPRINTF(buf + ret , buf_len - ret, "net name:%s\n",netdev_name(pst_net_device));
            ret +=  OAL_SPRINTF(buf + ret , buf_len - ret, "tx [%d]queues info,state [bit0:DRV_OFF],[bit1:STACK_OFF],[bit2:FROZEN]\n",
                                                        pst_net_device->num_tx_queues);
            for (i = 0; i < pst_net_device->num_tx_queues; i++) {
                struct netdev_queue *txq = netdev_get_tx_queue(pst_net_device, i);
                if(txq->state)
                    ret +=  OAL_SPRINTF(buf + ret , buf_len - ret, "net queue[%2d]'s state:0x%lx\n",i,txq->state);
            }
        }
        ret +=  OAL_SPRINTF(buf + ret , buf_len - ret, "\n");
    }
    return ret;
}

OAL_STATIC ssize_t  hmac_get_vap_stat(struct kobject *dev, struct kobj_attribute *attr, char* buf)
{
    int ret = 0;

    if (NULL == buf)
    {
        OAL_IO_PRINT("buf is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == attr)
    {
        OAL_IO_PRINT("attr is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == dev)
    {
        OAL_IO_PRINT("dev is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    ret += hmac_print_vap_stat(NULL,buf,PAGE_SIZE - ret);
    return ret;
}
OAL_STATIC struct kobj_attribute dev_attr_vap_info =
    __ATTR(vap_info, S_IRUGO, hmac_get_vap_stat, NULL);

OAL_STATIC ssize_t  hmac_get_adapt_info(struct kobject *dev, struct kobj_attribute *attr, char* buf)
{
    int ret = 0;
    if (NULL == buf)
    {
        OAL_IO_PRINT("buf is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == attr)
    {
        OAL_IO_PRINT("attr is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == dev)
    {
        OAL_IO_PRINT("dev is null r failed!%s\n",__FUNCTION__);
        return 0;
    }


    ret += hmac_tx_event_pkts_info_print(NULL, buf + ret,PAGE_SIZE - ret);
    return ret;
}

oal_int32 hmac_wakelock_info_print(char* buf, oal_int32 buf_len)
{
    oal_int32 ret = 0;

#ifdef CONFIG_PRINTK
    if(g_st_hmac_wakelock.locked_addr)
    {
        ret +=  OAL_SPRINTF(buf + ret , buf_len - ret,"wakelocked by:%pf\n",
                    (oal_void*)g_st_hmac_wakelock.locked_addr);
    }
#endif

    ret +=  OAL_SPRINTF(buf + ret , buf_len - ret,"hold %lu locks\n", g_st_hmac_wakelock.lock_count);

    return ret;
}

OAL_STATIC ssize_t  hmac_get_wakelock_info(struct kobject *dev, struct kobj_attribute *attr, char*buf)
{
    int ret = 0;
    if (NULL == buf)
    {
        OAL_IO_PRINT("buf is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == attr)
    {
        OAL_IO_PRINT("attr is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == dev)
    {
        OAL_IO_PRINT("dev is null r failed!%s\n",__FUNCTION__);
        return 0;
    }


    ret += hmac_wakelock_info_print(buf,PAGE_SIZE - ret);

    return ret;
}

OAL_STATIC struct kobj_attribute dev_attr_adapt_info =
    __ATTR(adapt_info, S_IRUGO, hmac_get_adapt_info, NULL);
OAL_STATIC struct kobj_attribute dev_attr_wakelock =
    __ATTR(wakelock, S_IRUGO, hmac_get_wakelock_info, NULL);

OAL_STATIC ssize_t  hmac_show_roam_status(struct kobject *dev, struct kobj_attribute *attr, char* buf)
{
    oal_int32               ret = 0;
    oal_uint8               uc_vap_id;
    oal_uint8               uc_roming_now  = 0;
    hmac_vap_stru          *pst_hmac_vap   = NULL;
#ifdef _PRE_WLAN_FEATURE_WAPI
    hmac_user_stru         *pst_hmac_user_multi;
#endif
#ifdef _PRE_WLAN_FEATURE_BTCOEX
    hmac_user_stru *pst_hmac_user;
#endif


    if (OAL_PTR_NULL == buf)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{buf is NULL.}");
        return ret;
    }

    if ((OAL_PTR_NULL == dev) || (OAL_PTR_NULL == attr))
    {
        ret +=  OAL_SPRINTF(buf , PAGE_SIZE, "roam_status=0\n");
        return ret;
    }

    for (uc_vap_id = 0; uc_vap_id < WLAN_VAP_MAX_NUM_PER_DEVICE_LIMIT; uc_vap_id++)
    {
        pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(uc_vap_id);

        if(OAL_PTR_NULL == pst_hmac_vap)
        {
            continue;
        }

        if ((WLAN_VAP_MODE_BSS_STA != pst_hmac_vap->st_vap_base_info.en_vap_mode)
            || (MAC_VAP_STATE_BUTT == pst_hmac_vap->st_vap_base_info.en_vap_state))
        {
            continue;
        }

        if (MAC_VAP_STATE_ROAMING == pst_hmac_vap->st_vap_base_info.en_vap_state)
        {
            uc_roming_now = 1;
            break;
        }

#ifdef _PRE_WLAN_FEATURE_WAPI
        /* wapi�£���roam��־��Ϊ1����ֹarp̽�� */
        pst_hmac_user_multi = (hmac_user_stru *)mac_res_get_hmac_user(pst_hmac_vap->st_vap_base_info.us_multi_user_idx);
        if (OAL_PTR_NULL == pst_hmac_user_multi)
        {
            continue;
        }

        if (OAL_TRUE == pst_hmac_user_multi->st_wapi.uc_port_valid)
        {
            uc_roming_now = 1;
            break;
        }
#endif/* #ifdef _PRE_WLAN_FEATURE_WAPI */

#ifdef _PRE_WLAN_FEATURE_BTCOEX
        pst_hmac_user = (hmac_user_stru *)mac_res_get_hmac_user(pst_hmac_vap->st_vap_base_info.uc_assoc_vap_id);
        if (OAL_PTR_NULL == pst_hmac_user)
        {
            continue;
        }

        if (OAL_TRUE == hmac_btcoex_check_by_ba_size(pst_hmac_user))
        {
            uc_roming_now = 1;
        }
#endif

    }
    /* �ȳ�һ���汾ǿ�ƹر�arp̽�⣬������Ч�� */
    //uc_roming_now = 1;

    ret += OAL_SPRINTF(buf , PAGE_SIZE, "roam_status=%1d\n", uc_roming_now);
    return ret;
}

OAL_STATIC struct kobj_attribute dev_attr_roam_status =
    __ATTR(roam_status, S_IRUGO, hmac_show_roam_status, NULL);


OAL_STATIC ssize_t hmac_set_rxthread_enable(struct kobject *dev, struct kobj_attribute *attr, const char *buf, size_t count)
{
    oal_uint32 ul_val;

    if (NULL == buf)
    {
        OAL_IO_PRINT("buf is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == attr)
    {
        OAL_IO_PRINT("attr is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == dev)
    {
        OAL_IO_PRINT("dev is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (sscanf(buf, "%u", &ul_val) != 1)
    {
        OAL_IO_PRINT("set value one char!\n");
        return -OAL_EINVAL;
    }

    g_st_rxdata_thread.en_rxthread_enable = (oal_bool_enum_uint8)ul_val;

    return count;
}
OAL_STATIC ssize_t hmac_get_rxthread_info(struct kobject *dev, struct kobj_attribute *attr, char* buf)
{
    int ret = 0;
    if (NULL == buf)
    {
        OAL_IO_PRINT("buf is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == attr)
    {
        OAL_IO_PRINT("attr is null r failed!%s\n",__FUNCTION__);
        return 0;
    }

    if (NULL == dev)
    {
        OAL_IO_PRINT("dev is null r failed!%s\n",__FUNCTION__);
        return 0;
    }


    ret += OAL_SPRINTF(buf , PAGE_SIZE, "rxthread_enable=%d\nrxthread_queue_len=%d\nrxthread_pkt_loss=%d\n",
            g_st_rxdata_thread.en_rxthread_enable, oal_netbuf_list_len(&g_st_rxdata_thread.st_rxdata_netbuf_head), g_st_rxdata_thread.ul_pkt_loss_cnt);

    return ret;
}
OAL_STATIC struct kobj_attribute dev_attr_rxdata_info =
    __ATTR(rxdata_info, S_IRUGO|S_IWUSR, hmac_get_rxthread_info, hmac_set_rxthread_enable);

OAL_STATIC struct attribute *hmac_sysfs_entries[] = {
        &dev_attr_vap_info.attr,
        &dev_attr_adapt_info.attr,
        &dev_attr_wakelock.attr,
        &dev_attr_roam_status.attr,
        &dev_attr_rxdata_info.attr,
        NULL
};

OAL_STATIC struct attribute_group hmac_attribute_group = {
        .name = "vap",
        .attrs = hmac_sysfs_entries,
};

OAL_STATIC oal_int32 hmac_sysfs_entry_init(oal_void)
{
    oal_int32       ret = OAL_SUCC;
    oal_kobject    *pst_root_object = NULL;
    pst_root_object = oal_get_sysfs_root_object();
    if(NULL == pst_root_object)
    {
        OAM_ERROR_LOG0(0,OAM_SF_ANY,"{hmac_sysfs_entry_init::get sysfs root object failed!}");
        return -OAL_EFAIL;
    }

    g_conn_syfs_hmac_object = kobject_create_and_add("hmac", pst_root_object);
    if(NULL == g_conn_syfs_hmac_object)
    {
        OAM_ERROR_LOG0(0,OAM_SF_ANY,"{hmac_sysfs_entry_init::create hmac object failed!}");
        return -OAL_EFAIL;
    }

    ret = sysfs_create_group(g_conn_syfs_hmac_object, &hmac_attribute_group);
    if (ret)
    {
        kobject_put(g_conn_syfs_hmac_object);
        OAM_ERROR_LOG0(0,OAM_SF_ANY,"{hmac_sysfs_entry_init::sysfs create group failed!}");
        return ret;
    }
    return OAL_SUCC;
}

OAL_STATIC oal_int32 hmac_sysfs_entry_exit(oal_void)
{
    if(g_conn_syfs_hmac_object)
    {
        sysfs_remove_group(g_conn_syfs_hmac_object, &hmac_attribute_group);
        kobject_put(g_conn_syfs_hmac_object);
    }
    return OAL_SUCC;
}
#endif

#ifdef _PRE_CONFIG_HISI_PANIC_DUMP_SUPPORT
OAL_STATIC DECLARE_WIFI_PANIC_STRU(hmac_panic_vap_stat,hmac_print_vap_stat);
#endif

oal_bool_enum_uint8 hmac_get_pm_pause_func(oal_void)
{
    hmac_device_stru    *pst_hmac_device;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_FEATURE_STA_PM
    oal_uint8            uc_vap_idx;
    hmac_vap_stru       *pst_hmac_vap;
    oal_bool_enum_uint8  uc_is_any_cnt_exceed_limit = OAL_FALSE;
    oal_bool_enum_uint8  uc_is_any_timer_registerd  = OAL_FALSE;
#endif
#endif
    /* ��ȡmac device�ṹ��ָ�� */
    pst_hmac_device = hmac_res_get_mac_dev(0);

    if (OAL_PTR_NULL == pst_hmac_device)
    {
       OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_get_pm_pause_func::pst_device null.}");

       return OAL_FALSE;
    }

    if(OAL_TRUE == pst_hmac_device->st_scan_mgmt.en_is_scanning)
    {
        OAM_INFO_LOG0(0, OAM_SF_ANY, "{hmac_get_pm_pause_func::in scanning}");
        return  OAL_TRUE;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_FEATURE_STA_PM
    for (uc_vap_idx = 0; uc_vap_idx < pst_hmac_device->pst_device_base_info->uc_vap_num; uc_vap_idx++)
    {
        pst_hmac_vap = (hmac_vap_stru *)mac_res_get_hmac_vap(pst_hmac_device->pst_device_base_info->auc_vap_id[uc_vap_idx]);
        if (OAL_PTR_NULL == pst_hmac_vap)
        {
            OAM_ERROR_LOG0(0, OAM_SF_CFG, "{hmac_get_pm_pause_func::pst_hmac_vap null.}");
            return OAL_FALSE;
        }

        if (OAL_TRUE == pst_hmac_vap->st_ps_sw_timer.en_is_registerd)
        {
            uc_is_any_timer_registerd = OAL_TRUE;
            pst_hmac_vap->ul_check_timer_pause_cnt ++;
            if (pst_hmac_vap->ul_check_timer_pause_cnt > HMAC_SWITCH_STA_PSM_MAX_CNT)
            {
                uc_is_any_cnt_exceed_limit = OAL_TRUE;
                OAM_ERROR_LOG2(0, OAM_SF_CFG, "{hmac_get_pm_pause_func::sw ps timer cnt too large[%d]> max[%d]}", pst_hmac_vap->ul_check_timer_pause_cnt, HMAC_SWITCH_STA_PSM_MAX_CNT);
            }
        }
        else
        {
            if (pst_hmac_vap->ul_check_timer_pause_cnt != 0)
            {

                OAM_WARNING_LOG2(0, OAM_SF_CFG, "{hmac_get_pm_pause_func::g_uc_check_timer_cnt end[%d],max[%d]", pst_hmac_vap->ul_check_timer_pause_cnt, HMAC_SWITCH_STA_PSM_MAX_CNT);
            }
            pst_hmac_vap->ul_check_timer_pause_cnt = 0;
        }
    }

    if (uc_is_any_cnt_exceed_limit)
    {
        return OAL_FALSE;
    }
    else if (uc_is_any_timer_registerd)
    {
        return OAL_TRUE;
    }
#endif
#endif
    return OAL_FALSE;

}

oal_void hmac_register_pm_callback(oal_void)
{
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)&&(_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)

    struct wifi_srv_callback_handler *pst_wifi_srv_handler;

    pst_wifi_srv_handler = wlan_pm_get_wifi_srv_handler();

    if(pst_wifi_srv_handler)
    {
        pst_wifi_srv_handler->p_wifi_srv_get_pm_pause_func = hmac_get_pm_pause_func;
        pst_wifi_srv_handler->p_wifi_srv_open_notify       = hmac_wifi_state_notify;
        pst_wifi_srv_handler->p_wifi_srv_pm_state_notify   = hmac_wifi_pm_state_notify;
    }
    else
    {
        OAL_IO_PRINT("hmac_register_pm_callback:wlan_pm_get_wifi_srv_handler is null\n");
    }
#endif

}

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
oal_bool_enum_uint8 hmac_get_rxthread_enable(oal_void)
{
    return g_st_rxdata_thread.en_rxthread_enable;
}

oal_void hmac_rxdata_sched(oal_void)
{


#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    up(&g_st_rxdata_thread.st_rxdata_sema);
#endif
    return;
}

oal_void hmac_rxdata_update_napi_weight(oal_netdev_priv_stru  * pst_netdev_priv)
{
    oal_uint32    ul_now;
    oal_uint8     uc_new_napi_weight;

    /* ����ppsˮ�ߵ���napi weight,��������1s */
    ul_now = (oal_uint32)OAL_TIME_GET_STAMP_MS();
    if( OAL_TIME_GET_RUNTIME(pst_netdev_priv->ul_period_start, ul_now) > NAPI_STAT_PERIOD)
    {
        pst_netdev_priv->ul_period_start = ul_now;
        if (pst_netdev_priv->ul_period_pkts < NAPI_WATER_LINE_LEV1)
        {
            uc_new_napi_weight = NAPI_POLL_WEIGHT_LEV1;
        }
        else if (pst_netdev_priv->ul_period_pkts < NAPI_WATER_LINE_LEV2)
        {
            uc_new_napi_weight = NAPI_POLL_WEIGHT_LEV2;
        }
        else if (pst_netdev_priv->ul_period_pkts < NAPI_WATER_LINE_LEV3)
        {
            uc_new_napi_weight = NAPI_POLL_WEIGHT_LEV3;
        }
        else
        {
            uc_new_napi_weight = NAPI_POLL_WEIGHT_MAX;
        }
        if (uc_new_napi_weight != pst_netdev_priv->uc_napi_weight)
        {
            pst_netdev_priv->uc_napi_weight = uc_new_napi_weight;
            pst_netdev_priv->st_napi.weight = uc_new_napi_weight;
            OAM_WARNING_LOG2(0, OAM_SF_CFG, "{hmac_rxdata_update_napi_weight: new_napi_weight[%d],totoal pkts[%d]}",uc_new_napi_weight,pst_netdev_priv->ul_period_pkts);
        }
        pst_netdev_priv->ul_period_pkts = 0;
    }
}
oal_void hmac_rxdata_netbuf_enqueue(oal_netbuf_stru  *pst_netbuf)
{
    oal_netdev_priv_stru *pst_netdev_priv;

    pst_netdev_priv = (oal_netdev_priv_stru *)OAL_NET_DEV_WIRELESS_PRIV(pst_netbuf->dev);

    if (pst_netdev_priv->ul_queue_len_max < oal_netbuf_list_len(&pst_netdev_priv->st_rx_netbuf_queue))
    {
        oal_netbuf_free(pst_netbuf);
        return;
    }
    oal_netbuf_list_tail(&pst_netdev_priv->st_rx_netbuf_queue, pst_netbuf);
    pst_netdev_priv->ul_period_pkts++;
}

OAL_STATIC oal_int32 hmac_rxdata_thread(oal_void* p_data)
{

    oal_netbuf_stru      *pst_netbuf = OAL_PTR_NULL;
    mac_device_stru      *pst_mac_device;
    oal_uint8            uc_vap_idx;
    hmac_vap_stru        *pst_hmac_vap;
    oal_netdev_priv_stru *pst_netdev_priv;
    struct sched_param       param;

    param.sched_priority = 97;
    oal_set_thread_property(current,
                            SCHED_FIFO,
                            &param,
                            -10);

    allow_signal(SIGTERM);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    while(OAL_LIKELY(!down_interruptible(&g_st_rxdata_thread.st_rxdata_sema)))
#else
    for(;;)
#endif
    {
        if (oal_kthread_should_stop())
        {
            break;
        }

        pst_mac_device = mac_res_get_dev(0);

        for(uc_vap_idx = 0; uc_vap_idx < pst_mac_device->uc_vap_num; uc_vap_idx++)
        {
            pst_hmac_vap = mac_res_get_hmac_vap(pst_mac_device->auc_vap_id[uc_vap_idx]);
            if( OAL_PTR_NULL == pst_hmac_vap)
            {
                continue;
            }

            if( OAL_PTR_NULL == pst_hmac_vap->pst_net_device)
            {
                continue;
            }

            pst_netdev_priv = (oal_netdev_priv_stru *)OAL_NET_DEV_WIRELESS_PRIV(pst_hmac_vap->pst_net_device);

            if( 0 == oal_netbuf_list_len(&pst_netdev_priv->st_rx_netbuf_queue))
            {
                continue;
            }

            if( OAL_TRUE == pst_netdev_priv->uc_napi_enable)
            {
                /*NAPI����Ȩ��*/
                if(pst_netdev_priv->uc_napi_dyn_weight)
                {
                    hmac_rxdata_update_napi_weight(pst_netdev_priv);
                }
                /*NAPI�ύ�ں�*/
                oal_napi_schedule(&pst_netdev_priv->st_napi);
            }
            else
            {
            /*netif_rx�ύ�ں�*/
                pst_netbuf = oal_netbuf_delist(&pst_netdev_priv->st_rx_netbuf_queue);
                if( pst_netbuf)
                {

                    oal_netif_rx_ni(pst_netbuf);
                }
            }
        }
    }
    return OAL_SUCC;
}


oal_int32   hmac_rxdata_polling(struct napi_struct *pst_napi, oal_int32 l_weight)
{
    oal_netbuf_stru      *pst_netbuf = OAL_PTR_NULL;
    oal_netdev_priv_stru *pst_netdev_priv;
    oal_int32             l_rx_num = 0;

    pst_netdev_priv = OAL_CONTAINER_OF(pst_napi, oal_netdev_priv_stru, st_napi);

    while (l_rx_num < l_weight)
    {
        pst_netbuf = oal_netbuf_delist(&pst_netdev_priv->st_rx_netbuf_queue);
        if (OAL_PTR_NULL == pst_netbuf)
        {
            break;
        }
        if (OAL_TRUE == pst_netdev_priv->uc_gro_enable)
        {
            oal_napi_gro_receive(pst_napi, pst_netbuf);
        }
        else
        {
            oal_netif_receive_skb(pst_netbuf);
        }
        l_rx_num++;
    }
    if (l_rx_num < l_weight)
    {
        oal_napi_complete(pst_napi);
    }
    return l_rx_num;
}

OAL_STATIC oal_uint32 hmac_hisi_thread_init(oal_void)
{
#if defined(_PRE_WLAN_TCP_OPT) && !defined(WIN32)
    hmac_set_hmac_tcp_ack_process_func(hmac_tcp_ack_process);
    hmac_set_hmac_tcp_ack_need_schedule(hmac_tcp_ack_need_schedule);

    hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread = oal_thread_create(hcc_transfer_thread,
                                        hcc_get_110x_handler(),
                                        NULL,
                                        "hisi_hcc",
                                        HCC_TRANS_THREAD_POLICY,
                                        HCC_TRANS_THERAD_PRIORITY,
                                        -1);
    /* ��CPU1~7 */
    oal_set_thread_affinity(hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread);
    if(!hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread)
    {
        OAL_IO_PRINT("hisi_hcc thread create failed!\n");
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        mutex_destroy(&hcc_get_110x_handler()->tx_transfer_lock);
#endif
        return OAL_FAIL;
    }
#endif

    OAL_WAIT_QUEUE_INIT_HEAD(&g_st_rxdata_thread.st_rxdata_wq);

    oal_netbuf_list_head_init(&g_st_rxdata_thread.st_rxdata_netbuf_head);
    oal_spin_lock_init(&g_st_rxdata_thread.st_lock);
    g_st_rxdata_thread.en_rxthread_enable = OAL_TRUE;
    g_st_rxdata_thread.ul_pkt_loss_cnt    = 0;

    g_st_rxdata_thread.pst_rxdata_thread = oal_thread_create(hmac_rxdata_thread,
                                            NULL,
                                            &g_st_rxdata_thread.st_rxdata_sema,
                                            "hisi_rxdata",
                                            SCHED_FIFO,
                                            97,
                                            -1);
#ifndef WIN32
    /* ��CPU1~7 */
    oal_set_thread_affinity(g_st_rxdata_thread.pst_rxdata_thread);

    if (OAL_PTR_NULL == g_st_rxdata_thread.pst_rxdata_thread)
    {
        OAL_IO_PRINT("hisi_rxdata thread create failed!\n");
        oal_thread_stop(hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread, NULL);
        hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread = NULL;
        return OAL_FAIL;
    }
#endif
    return OAL_SUCC;
}

OAL_STATIC oal_void hmac_hisi_thread_exit(oal_void)
{
#if defined(_PRE_WLAN_TCP_OPT) && !defined(WIN32)
    if (hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread)
    {
        oal_thread_stop(hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread, NULL);
        hcc_get_110x_handler()->hcc_transer_info.hcc_transfer_thread = NULL;
    }
#endif
    if (g_st_rxdata_thread.pst_rxdata_thread)
    {
        oal_thread_stop(g_st_rxdata_thread.pst_rxdata_thread, &g_st_rxdata_thread.st_rxdata_sema);
        g_st_rxdata_thread.pst_rxdata_thread = NULL;
    }
}
#endif

#if defined(_PRE_FEATURE_PLAT_LOCK_CPUFREQ) && !defined(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)

#if defined(_PRE_FEATURE_PLAT_LOCK_CPUFREQ) && !defined(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)
oal_uint32 hisi_cpufreq_get_maxfreq(unsigned int cpu)
{
    struct cpufreq_policy *policy = cpufreq_cpu_get(cpu);
    oal_uint32 ul_max_freq;

    if (!policy)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "hisi_cpufreq_get_maxfreq: get cpufreq policy fail!");
        return 0;
    }

    ul_max_freq = (policy->cpuinfo).max_freq;

    /* ��СCPU���ü��� */
    cpufreq_cpu_put(policy);
    return ul_max_freq;
}

#endif
OAL_STATIC oal_uint32 hmac_hisi_cpufreq_init(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_uint8    uc_cpuid;

    for(uc_cpuid=0; uc_cpuid<OAL_BUS_MAXCPU_NUM; uc_cpuid++)
    {
        hisi_cpufreq_init_req(&g_ast_cpufreq[uc_cpuid], uc_cpuid);
        g_aul_cpumaxfreq[uc_cpuid] = hisi_cpufreq_get_maxfreq(uc_cpuid);
    }

    /* ����Pm Qos cpu idle latecy */
    pm_qos_add_request(&g_st_pmqos_requset, PM_QOS_CPU_DMA_LATENCY, PM_QOS_DEFAULT_VALUE);
    
#endif
    return OAL_SUCC;
}
OAL_STATIC oal_uint32 hmac_hisi_cpufreq_exit(oal_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_uint8    uc_cpuid;

    for(uc_cpuid=0; uc_cpuid<OAL_BUS_MAXCPU_NUM; uc_cpuid++)
    {
        hisi_cpufreq_exit_req(&g_ast_cpufreq[uc_cpuid]);
    }
#endif
    /* ɾ��Pm Qos cpu idle latecy */
    pm_qos_remove_request(&g_st_pmqos_requset);

    return OAL_SUCC;
}
#endif


oal_int32  hmac_main_init(oal_void)
{
    oal_uint32            ul_return;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

#endif
    frw_init_enum_uint16  en_init_state;
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC != _PRE_MULTI_CORE_MODE)
    frw_event_mem_stru   *pst_event_mem;
    frw_event_stru       *pst_event;
#endif

    oal_wake_lock_init(&g_st_hmac_wakelock, "wlan_hmac_wakelock");

    /* Ϊ�˽��ģ�������ʱ�䣬����ʱ�����ӡ TBD */
    if(OAL_SUCC != hmac_param_check())
    {
        OAL_IO_PRINT("hmac_main_init:hmac_param_check failed!\n");
        return -OAL_EFAIL;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_hcc_adapt_init();
#endif

    en_init_state = frw_get_init_state();

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    if (OAL_UNLIKELY((FRW_INIT_STATE_BUTT == en_init_state) || (en_init_state < FRW_INIT_STATE_FRW_SUCC)))
#else
    if (OAL_UNLIKELY((FRW_INIT_STATE_BUTT == en_init_state) || (en_init_state < FRW_INIT_STATE_DMAC_CONFIG_VAP_SUCC)))
#endif
    {
        OAL_IO_PRINT("hmac_main_init:en_init_state is error %d\n", en_init_state);
        frw_timer_delete_all_timer();
        return -OAL_EFAIL;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    ul_return = hmac_hisi_thread_init();
    if (OAL_SUCC != ul_return)
    {
        frw_timer_delete_all_timer();
        OAL_IO_PRINT("hmac_main_init: hmac_hisi_thread_init failed\n");
        return -OAL_EFAIL;
    }

#if defined(_PRE_FEATURE_PLAT_LOCK_CPUFREQ) && !defined(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)
    hmac_hisi_cpufreq_init();
#endif
    ul_return = mac_res_init();
    if (OAL_SUCC != ul_return)
    {
        OAL_IO_PRINT("hmac_main_init: mac_res_init return err code %d\n", ul_return);
        frw_timer_delete_all_timer();
        return -OAL_EFAIL;
    }
#endif

    /* hmac��Դ��ʼ�� */
    hmac_res_init();

    /* �����ʼ��״̬��������VAP�ɹ�ǰ��״̬�������˴�ΪHMAC��һ�γ�ʼ�������ؼ��ػ�������ʼ�� */
    if (en_init_state < FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC)
    {
        /* ����״̬����ʼ���ӿ� */
        hmac_fsm_init();

        hmac_rx_filter_init();

        /* �¼�ע�� */
        hmac_event_fsm_register();

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
        /* DMAC OFFLOAD�²���Ҫ���¼�����device��ȡchip��Ϣ */
        ul_return = hmac_board_init(&g_st_hmac_board);
        if (OAL_SUCC != ul_return)
        {
            frw_timer_delete_all_timer();
            event_fsm_unregister();
            mac_res_exit();
            hmac_res_exit(&g_st_hmac_board);  /* �ͷ�hmac res��Դ */
            return OAL_FAIL;
        }
#else
        /* ���¼���dmac */
        pst_event_mem = FRW_EVENT_ALLOC(0);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
        {
            OAL_IO_PRINT("hmac_main_init: FRW_EVENT_ALLOC result = OAL_PTR_NULL.\n");
            frw_timer_delete_all_timer();
            return -OAL_EFAIL;
        }

        pst_event = (frw_event_stru *)pst_event_mem->puc_data;

        /* ��д�¼�ͷ */
        FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                        FRW_EVENT_TYPE_HOST_CRX,
                        HMAC_TO_DMAC_SYN_INIT,
                        0,
                        FRW_EVENT_PIPELINE_STAGE_1,
                        0,0,0);
        ul_return = frw_event_dispatch_event(pst_event_mem);
        if (OAL_SUCC != ul_return)
        {
            OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_main_init::frw_event_dispatch_event failed[%d].}", ul_return);
        }

        /* �ͷ��¼� */
        FRW_EVENT_FREE(pst_event_mem);
#endif

        frw_set_init_state(FRW_INIT_STATE_HMAC_CONFIG_VAP_SUCC);

        /* �����ɹ��������ӡ */
    }
    else
    {
        /* TBD ����10 �������޸� �����ʼ��״̬Ϊ����VAP�ɹ����״̬�����������Ϊ���ô����ĳ�ʼ����
           ��Ҫ��������ҵ��VAP���������״̬����δ��ʼ��������Ҫ��ʼ�����������
           ��֧�����Խӿڹҽ� */
    }

#ifdef _PRE_WLAN_CHIP_TEST
    hmac_test_init();
#endif

#ifdef _PRE_WLAN_FEATURE_DAQ
    hmac_data_acq_init();
#endif

    /* DFX ģ���ʼ�� */
    hmac_dfx_init();

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102A_HOST)
    hmac_auto_ddr_init();
#endif

#if defined(_PRE_WLAN_CHIP_TEST_ALG)
    hmac_alg_test_main_common_init();
#endif
#endif

#ifdef _PRE_WLAN_PROFLING_MIPS
    oal_profiling_mips_init();
#endif

#if defined(_PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_sysfs_entry_init();
#endif

#ifdef _PRE_CONFIG_HISI_PANIC_DUMP_SUPPORT
    hwifi_panic_log_register(&hmac_panic_vap_stat,NULL);
#endif

    hmac_register_pm_callback();

#ifdef _PRE_SUPPORT_ACS
    hmac_acs_init();
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    hmac_psta_mgr_init();
#endif
    return OAL_SUCC;
}


oal_void  hmac_main_exit(oal_void)
{
    oal_uint32 ul_return;

    // TODO:  1102A host��ƽ̨����
#ifdef _PRE_CONFIG_HISI_PANIC_DUMP_SUPPORT
    hwifi_panic_log_unregister(&hmac_panic_vap_stat);
#endif

#if defined(_PRE_CONFIG_CONN_HISI_SYSFS_SUPPORT) && (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_sysfs_entry_exit();
#endif
    event_fsm_unregister();

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_hisi_thread_exit();
#endif

#if defined(_PRE_FEATURE_PLAT_LOCK_CPUFREQ) && !defined(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT)
    hmac_hisi_cpufreq_exit();
#endif









    ul_return = hmac_board_exit(&g_st_hmac_board);
    if (OAL_UNLIKELY(OAL_SUCC != ul_return))
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_main_exit::hmac_board_exit failed[%d].}", ul_return);
        return ;
    }


    mac_res_exit();

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    hmac_hcc_adapt_deinit();
#endif

#ifdef _PRE_WLAN_FEATURE_DAQ
    hmac_data_acq_exit();
#endif
    /* DFX ģ���ʼ�� */
    hmac_dfx_exit();

    hmac_rx_filter_exit();

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    frw_set_init_state(FRW_INIT_STATE_FRW_SUCC);
#else
    frw_set_init_state(FRW_INIT_STATE_DMAC_CONFIG_VAP_SUCC);
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#if (_PRE_PRODUCT_ID == _PRE_PRODUCT_ID_HI1102A_HOST)
    hmac_auto_ddr_exit();
#endif

#if defined(_PRE_WLAN_CHIP_TEST_ALG)
    hmac_alg_test_main_common_exit();
#endif
#endif

    oal_wake_lock_exit(&g_st_hmac_wakelock);

#ifdef _PRE_SUPPORT_ACS
    hmac_acs_exit();
#endif

#ifdef _PRE_WLAN_FEATURE_PROXYSTA
    hmac_psta_mgr_exit();
#endif

}


oal_uint32  hmac_sdt_recv_reg_cmd(
                            mac_vap_stru  *pst_mac_vap,
                            oal_uint8     *puc_buf,
                            oal_uint16     us_len)
{
    frw_event_mem_stru          *pst_event_mem;
    frw_event_stru              *pst_event;

    pst_event_mem = FRW_EVENT_ALLOC(us_len - OAL_IF_NAME_SIZE);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_sdt_recv_reg_cmd::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = (frw_event_stru *)pst_event_mem->puc_data;

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CRX,
                       HMAC_TO_DMAC_SYN_REG,
                       (oal_uint16)(us_len - OAL_IF_NAME_SIZE),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    oal_memcopy(pst_event->auc_event_data, puc_buf + OAL_IF_NAME_SIZE, us_len - OAL_IF_NAME_SIZE);

    frw_event_dispatch_event(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}


oal_uint32  hmac_sdt_up_reg_val(frw_event_mem_stru  *pst_event_mem)
{
    frw_event_stru      *pst_event;
    hmac_vap_stru       *pst_hmac_vap;
    oal_uint32          *pst_reg_val;

    pst_event = (frw_event_stru  *)pst_event_mem->puc_data;

    pst_hmac_vap = mac_res_get_hmac_vap(pst_event->st_event_hdr.uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_event->st_event_hdr.uc_vap_id, OAM_SF_ANY, "{hmac_sdt_up_reg_val::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_reg_val = (oal_uint32 *)pst_hmac_vap->st_cfg_priv.ac_rsp_msg;
   *pst_reg_val = *((oal_uint32 *)pst_event->auc_event_data);

   /* ����wal_sdt_recv_reg_cmd�ȴ��Ľ��� */
   pst_hmac_vap->st_cfg_priv.en_wait_ack_for_sdt_reg = OAL_TRUE;
   OAL_WAIT_QUEUE_WAKE_UP_INTERRUPT(&(pst_hmac_vap->st_cfg_priv.st_wait_queue_for_sdt_reg));

   return OAL_SUCC;
}

#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE)

oal_uint32  hmac_sdt_recv_sample_cmd(
                            mac_vap_stru  *pst_mac_vap,
                            oal_uint8     *puc_buf,
                            oal_uint16     us_len)
{
    frw_event_mem_stru          *pst_event_mem;
    frw_event_stru              *pst_event;

    pst_event_mem = FRW_EVENT_ALLOC(us_len - OAL_IF_NAME_SIZE);
    if (OAL_UNLIKELY(OAL_PTR_NULL == pst_event_mem))
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_sdt_recv_reg_cmd::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_event = frw_get_event_stru(pst_event_mem);

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CRX,
                       HMAC_TO_DMAC_SYN_SAMPLE,
                       (oal_uint16)(us_len - OAL_IF_NAME_SIZE),
                       FRW_EVENT_PIPELINE_STAGE_1,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    oal_memcopy(pst_event->auc_event_data, puc_buf + OAL_IF_NAME_SIZE, us_len - OAL_IF_NAME_SIZE);

    frw_event_dispatch_event(pst_event_mem);
    FRW_EVENT_FREE(pst_event_mem);

    return OAL_SUCC;
}


oal_uint32  hmac_sdt_up_sample_data(frw_event_mem_stru  *pst_event_mem)
{
    oal_uint16                          us_payload_len;
    frw_event_stru                     *pst_event;
    frw_event_stru                     *pst_event_up;
    frw_event_mem_stru                 *pst_hmac_event_mem;
    frw_event_hdr_stru                 *pst_event_hdr;
    dmac_sdt_sample_frame_stru         *pst_sample_frame;
    dmac_sdt_sample_frame_stru         *pst_sample_frame_syn;
    frw_event_mem_stru                 *pst_syn_event_mem;
    frw_event_stru                     *pst_event_syn;

    if (OAL_PTR_NULL == pst_event_mem)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_sdt_up_sample_data::pst_event_mem null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��ȡ�¼�ͷ���¼��ṹ��ָ�� */
    pst_event           = frw_get_event_stru(pst_event_mem);
    pst_event_hdr       = &(pst_event->st_event_hdr);
    us_payload_len = pst_event_hdr->us_length - OAL_SIZEOF(frw_event_hdr_stru);

    /* �׵�WAL */
    pst_hmac_event_mem = FRW_EVENT_ALLOC(us_payload_len);
    if (OAL_PTR_NULL == pst_hmac_event_mem)
    {
       OAM_ERROR_LOG0(pst_event_hdr->uc_vap_id, OAM_SF_ANY, "{hmac_sdt_up_sample_data::pst_hmac_event_mem null.}");
       return OAL_ERR_CODE_PTR_NULL;
    }

    /* ��д�¼� */
    pst_event_up = frw_get_event_stru(pst_hmac_event_mem);

    FRW_EVENT_HDR_INIT(&(pst_event_up->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CTX,
                       HMAC_HOST_CTX_EVENT_SUB_TYPE_SAMPLE_REPORT,
                       us_payload_len,
                       FRW_EVENT_PIPELINE_STAGE_0,
                       pst_event_hdr->uc_chip_id,
                       pst_event_hdr->uc_device_id,
                       pst_event_hdr->uc_vap_id);

    oal_memcopy(pst_event_up->auc_event_data,(oal_uint8 *)frw_get_event_payload(pst_event_mem), us_payload_len);

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_hmac_event_mem);
    FRW_EVENT_FREE(pst_hmac_event_mem);

    pst_sample_frame = (dmac_sdt_sample_frame_stru *)pst_event->auc_event_data;

    if (pst_sample_frame->ul_count && pst_sample_frame->ul_count <= pst_sample_frame->ul_reg_num)
    {
        pst_syn_event_mem = FRW_EVENT_ALLOC(OAL_SIZEOF(dmac_sdt_sample_frame_stru));
        if (OAL_PTR_NULL == pst_syn_event_mem)
        {
           OAM_ERROR_LOG0(pst_event_hdr->uc_vap_id, OAM_SF_ANY, "{hmac_sdt_up_sample_data::pst_syn_event_mem null.}");
           return OAL_ERR_CODE_PTR_NULL;
        }
        pst_event_syn = frw_get_event_stru(pst_syn_event_mem);
        /* ��д�¼�ͷ */
        FRW_EVENT_HDR_INIT(&(pst_event_syn->st_event_hdr),
                          FRW_EVENT_TYPE_HOST_CRX,
                          HMAC_TO_DMAC_SYN_SAMPLE,
                          OAL_SIZEOF(dmac_sdt_sample_frame_stru),
                          FRW_EVENT_PIPELINE_STAGE_1,
                          pst_event_hdr->uc_chip_id,
                          pst_event_hdr->uc_device_id,
                          pst_event_hdr->uc_vap_id);


        pst_sample_frame_syn = (dmac_sdt_sample_frame_stru *)pst_event_syn->auc_event_data;
        pst_sample_frame_syn->ul_reg_num = pst_sample_frame->ul_reg_num;
        pst_sample_frame_syn->ul_count = pst_sample_frame->ul_count;
        pst_sample_frame_syn->ul_type = 0;
        frw_event_dispatch_event(pst_syn_event_mem);
        FRW_EVENT_FREE(pst_syn_event_mem);
    }

    return OAL_SUCC;
}
#endif

OAL_STATIC oal_uint32  hmac_create_ba_event(frw_event_mem_stru  *pst_event_mem)
{
    frw_event_stru      *pst_event;
    hmac_user_stru      *pst_hmac_user;
    hmac_vap_stru       *pst_hmac_vap;
    mac_tx_ctl_stru      st_cb;
    dmac_to_hmac_ctx_event_stru    *pst_create_ba_event;

    pst_event = (frw_event_stru  *)pst_event_mem->puc_data;

    pst_create_ba_event = (dmac_to_hmac_ctx_event_stru *)pst_event->auc_event_data;

    pst_hmac_user = mac_res_get_hmac_user(pst_create_ba_event->us_user_index);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_ERROR_LOG1(pst_event->st_event_hdr.uc_vap_id, OAM_SF_ANY, "{hmac_create_ba_event::pst_hmac_user[%d] null.}", pst_create_ba_event->us_user_index);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap  = mac_res_get_hmac_vap(pst_create_ba_event->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(pst_event->st_event_hdr.uc_vap_id, OAM_SF_ANY, "{hmac_create_ba_event::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    st_cb.uc_tid  = pst_create_ba_event->uc_tid;

    hmac_tx_ba_setup(pst_hmac_vap, pst_hmac_user, st_cb.uc_tid);

    return OAL_SUCC;

}


OAL_STATIC oal_uint32  hmac_del_ba_event(frw_event_mem_stru  *pst_event_mem)
{
    oal_uint8           uc_tid;
    frw_event_stru     *pst_event;
    hmac_user_stru     *pst_hmac_user;
    hmac_vap_stru      *pst_hmac_vap;

    mac_action_mgmt_args_stru       st_action_args;   /* ������дACTION֡�Ĳ��� */
    hmac_tid_stru                  *pst_hmac_tid;
    oal_uint32                      ul_ret;
    dmac_to_hmac_ctx_event_stru    *pst_del_ba_event;

    pst_event = (frw_event_stru  *)pst_event_mem->puc_data;

    pst_del_ba_event = (dmac_to_hmac_ctx_event_stru *)pst_event->auc_event_data;

    pst_hmac_user = mac_res_get_hmac_user(pst_del_ba_event->us_user_index);

    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_del_ba_event::pst_hmac_user[%d] null.}", pst_del_ba_event->us_user_index);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_hmac_vap  = mac_res_get_hmac_vap(pst_del_ba_event->uc_vap_id);
    if (OAL_PTR_NULL == pst_hmac_vap)
    {
        OAM_ERROR_LOG0(0, OAM_SF_ANY, "{hmac_del_ba_event::pst_hmac_vap null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }

#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)
    mac_user_set_cur_protocol_mode(&pst_hmac_user->st_user_base_info, pst_del_ba_event->uc_cur_protocol);
    ul_ret = hmac_config_user_info_syn(&(pst_hmac_vap->st_vap_base_info), &(pst_hmac_user->st_user_base_info));
    if (OAL_SUCC != ul_ret)
    {
        return ul_ret;
    }
#endif

    for (uc_tid = 0; uc_tid < WLAN_TID_MAX_NUM; uc_tid++)
    {
        pst_hmac_tid  = &pst_hmac_user->ast_tid_info[uc_tid];

        if (DMAC_BA_INIT == pst_hmac_tid->st_ba_tx_info.en_ba_status)
        {
            OAM_INFO_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY, "{hmac_del_ba_event::the tx hdl is not exist.}");
            continue;
        }

        st_action_args.uc_category = MAC_ACTION_CATEGORY_BA;
        st_action_args.uc_action   = MAC_BA_ACTION_DELBA;
        st_action_args.ul_arg1     = uc_tid;       /* ������֡��Ӧ��TID�� */
        st_action_args.ul_arg2     = MAC_ORIGINATOR_DELBA;           /* ADDBA_REQ�У�buffer_size��Ĭ�ϴ�С */
        st_action_args.ul_arg3     = MAC_UNSPEC_REASON;              /* BA�Ự��ȷ�ϲ��� */
        st_action_args.puc_arg5    = pst_hmac_user->st_user_base_info.auc_user_mac_addr;   /* ba�Ự��Ӧ��user */

        /* ɾ��BA�Ự */
        ul_ret = hmac_mgmt_tx_action(pst_hmac_vap,  pst_hmac_user, &st_action_args);
        if (OAL_SUCC != ul_ret)
        {
            OAM_WARNING_LOG0(pst_hmac_vap->st_vap_base_info.uc_vap_id, OAM_SF_ANY, "{hmac_del_ba_event::hmac_mgmt_tx_action failed.}");
            continue;
        }
    }

    return OAL_SUCC;

}
#if (_PRE_MULTI_CORE_MODE_OFFLOAD_DMAC == _PRE_MULTI_CORE_MODE)

OAL_STATIC oal_uint32  hmac_syn_info_event(frw_event_mem_stru  *pst_event_mem)
{
    frw_event_stru      *pst_event;
    hmac_user_stru      *pst_hmac_user;
    mac_vap_stru        *pst_mac_vap;
    oal_uint32           ul_relt;

    dmac_to_hmac_syn_info_event_stru    *pst_syn_info_event;

    pst_event = (frw_event_stru  *)pst_event_mem->puc_data;
    pst_syn_info_event = (dmac_to_hmac_syn_info_event_stru *)pst_event->auc_event_data;
    pst_hmac_user = mac_res_get_hmac_user(pst_syn_info_event->us_user_index);
    if (OAL_PTR_NULL == pst_hmac_user)
    {
        OAM_WARNING_LOG1(0, OAM_SF_ANY, "{hmac_syn_info_event: pst_hmac_user null,user_idx=%d.}", pst_syn_info_event->us_user_index);
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_mac_vap = mac_res_get_mac_vap(pst_hmac_user->st_user_base_info.uc_vap_id);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_WARNING_LOG2(0, OAM_SF_ANY, "{hmac_syn_info_event: pst_mac_vap null! vap_idx=%d, user_idx=%d.}",
                pst_hmac_user->st_user_base_info.uc_vap_id,
                pst_syn_info_event->us_user_index);
        return OAL_ERR_CODE_PTR_NULL;
    }

    mac_user_set_cur_protocol_mode(&pst_hmac_user->st_user_base_info, pst_syn_info_event->uc_cur_protocol);
    mac_user_set_cur_bandwidth(&pst_hmac_user->st_user_base_info, pst_syn_info_event->uc_cur_bandwidth);
    ul_relt = hmac_config_user_info_syn(pst_mac_vap, &pst_hmac_user->st_user_base_info);

    return ul_relt;

}


OAL_STATIC oal_uint32  hmac_voice_aggr_event(frw_event_mem_stru  *pst_event_mem)
{
    frw_event_stru      *pst_event;
    mac_vap_stru        *pst_mac_vap;

    dmac_to_hmac_voice_aggr_event_stru    *pst_voice_aggr_event;

    pst_event = (frw_event_stru  *)pst_event_mem->puc_data;
    pst_voice_aggr_event = (dmac_to_hmac_voice_aggr_event_stru *)pst_event->auc_event_data;

    pst_mac_vap = mac_res_get_mac_vap(pst_voice_aggr_event->uc_vap_id);
    if (OAL_PTR_NULL == pst_mac_vap)
    {
        OAM_ERROR_LOG1(0, OAM_SF_ANY, "{hmac_voice_aggr_event: pst_mac_vap null! vap_idx=%d}",
                pst_voice_aggr_event->uc_vap_id);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_vap->bit_voice_aggr = pst_voice_aggr_event->en_voice_aggr;

    return OAL_SUCC;
}
#endif

#ifdef _PRE_SUPPORT_ACS


oal_uint32 hmac_config_set_acs_cmd(mac_vap_stru *pst_mac_vap, oal_uint16 us_len, oal_uint8 *puc_param)
{
    mac_device_stru                *pst_mac_device;
    hmac_device_stru               *pst_hmac_device;
    oal_uint32                      ul_ret;
    mac_acs_cmd_stru               *pst_acs_cmd;

    /* ��ȡdevice */
    pst_hmac_device = hmac_res_get_mac_dev(pst_mac_vap->uc_device_id);

    if ((OAL_PTR_NULL == pst_hmac_device) || OAL_PTR_NULL == pst_hmac_device->pst_device_base_info || (OAL_PTR_NULL == puc_param))
    {
        OAM_ERROR_LOG2(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_set_acs_cmd::null param,pst_hmac_device=%x puc_param=%x}",
                       pst_hmac_device, puc_param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    pst_mac_device = pst_hmac_device->pst_device_base_info;

    if (pst_mac_device == OAL_PTR_NULL || OAL_PTR_NULL == pst_mac_device->pst_acs)
    {
        OAM_ERROR_LOG0(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_set_acs_cmd::pst_acs null.}");
        return OAL_ERR_CODE_PTR_NULL;
    }
    pst_acs_cmd = (mac_acs_cmd_stru *)puc_param;

    if (pst_acs_cmd->uc_cmd >=  DMAC_ACS_CMD_BUTT)
    {
        return OAL_SUCC;
    }

    if(pst_acs_cmd->uc_cmd == DMAC_ACS_CMD_DO_SCAN)
    {
        return hmac_init_scan_do(pst_hmac_device, pst_mac_vap, (mac_init_scan_req_stru *)pst_acs_cmd);
    }
    else if (pst_acs_cmd->uc_cmd == DMAC_ACS_CMD_SET_BEST_CHANNEL
             && pst_acs_cmd->auc_arg[0] == MAC_ACS_SET_CH_INIT)
    {
        return hmac_acs_got_init_rank(pst_hmac_device, pst_mac_vap, pst_acs_cmd);
    }

    /***************************************************************************
     ���¼���DMAC��, ͬ��DMAC����
    ***************************************************************************/
    ul_ret = hmac_config_send_event(pst_mac_vap, WLAN_CFGID_ACS_PARAM, us_len, puc_param);
    if (OAL_UNLIKELY(OAL_SUCC != ul_ret))
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_config_set_acs_cmd::hmac_config_send_event failed[%d].}", ul_ret);
    }

    return ul_ret;
}


oal_uint32 hmac_event_acs_response(frw_event_mem_stru  *pst_event_mem)
{
    frw_event_stru                     *pst_event;

    /* ת���¼���ֻ�޸����³�Ա */
    pst_event    = (frw_event_stru *)pst_event_mem->puc_data;
    pst_event->st_event_hdr.en_type = FRW_EVENT_TYPE_HOST_CTX;
    pst_event->st_event_hdr.uc_sub_type = HMAC_HOST_CTX_EVENT_SUB_TYPE_ACS_RESPONSE;
    pst_event->st_event_hdr.en_pipeline = FRW_EVENT_PIPELINE_STAGE_0;

    /* �ַ��¼� */
    frw_event_dispatch_event(pst_event_mem);

    return OAL_SUCC;
}
#endif /* #ifdef _PRE_SUPPORT_ACS */

#ifdef _PRE_WLAN_FEATURE_FLOWCTL

OAL_STATIC oal_uint32  hmac_flowctl_backp_event(frw_event_mem_stru  *pst_event_mem)
{
    frw_event_stru              *pst_hmac_to_wal_event;  /* ָ�������¼���payloadָ�� */
    mac_ioctl_queue_backp_stru  *pst_flowctl_backp_event;
    mac_vap_stru                *pst_mac_vap;
    oal_uint32                   ul_ret;
    hmac_user_stru              *pst_hmac_user;

    /* ����¼�ָ�� */
    pst_hmac_to_wal_event = (frw_event_stru *)pst_event_mem->puc_data;

    pst_flowctl_backp_event = (mac_ioctl_queue_backp_stru *)(pst_hmac_to_wal_event->auc_event_data);

    pst_mac_vap = (mac_vap_stru *) mac_res_get_mac_vap(pst_flowctl_backp_event->uc_vap_id);

    /* ��vap/user״̬����ͬ�� */
    if (0xFFFF == pst_flowctl_backp_event->us_assoc_id)
    {
        pst_mac_vap->bit_vap_bw_limit = pst_flowctl_backp_event->uc_is_stop;
    }
    else if (WLAN_TID_MAX_NUM == pst_flowctl_backp_event->uc_tidno)
    {
        pst_hmac_user = (hmac_user_stru *)mac_res_get_hmac_user(pst_flowctl_backp_event->us_assoc_id);
        if (OAL_UNLIKELY(OAL_PTR_NULL == pst_hmac_user))
        {
            OAM_WARNING_LOG1(0, OAM_SF_CFG, "{hmac_edca_opt_rx_pkts_stat::null param,pst_hmac_user[%d].}",pst_flowctl_backp_event->us_assoc_id);
            return OAL_ERR_CODE_PTR_NULL;
        }
        pst_hmac_user->en_user_bw_limit = (oal_bool_enum_uint8)(pst_flowctl_backp_event->uc_is_stop);
    }

    /* ��д�¼�ͷ */
    FRW_EVENT_HDR_INIT(&(pst_hmac_to_wal_event->st_event_hdr),
                       FRW_EVENT_TYPE_HOST_CTX,
                       HMAC_HOST_CTX_EVENT_SUB_TYPE_FLOWCTL_BACKP,
                       WLAN_MEM_EVENT_SIZE1,
                       FRW_EVENT_PIPELINE_STAGE_0,
                       pst_mac_vap->uc_chip_id,
                       pst_mac_vap->uc_device_id,
                       pst_mac_vap->uc_vap_id);

    /* �ַ� */
    ul_ret = frw_event_dispatch_event(pst_event_mem);
    if (OAL_SUCC != ul_ret)
    {
        OAM_WARNING_LOG1(pst_mac_vap->uc_vap_id, OAM_SF_ANY, "{hmac_flowctl_backp_event::frw_event_dispatch_event null.}", ul_ret);
        return ul_ret;
    }

    return OAL_SUCC;
}
#endif



/*lint -e578*//*lint -e19*/
oal_module_symbol(hmac_board_get_instance);
oal_module_symbol(hmac_sdt_recv_reg_cmd);
#if defined(_PRE_WLAN_FEATURE_DATA_SAMPLE)
oal_module_symbol(hmac_sdt_recv_sample_cmd);
#endif
#ifdef _PRE_SUPPORT_ACS
oal_module_symbol(hmac_config_set_acs_cmd);
#endif

oal_module_license("GPL");
/*lint +e578*//*lint +e19*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif