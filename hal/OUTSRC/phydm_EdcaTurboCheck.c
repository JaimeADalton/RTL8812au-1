/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

//============================================================
// include files
//============================================================
#include "Mp_Precomp.h"
#include "phydm_precomp.h"

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
static u4Byte edca_setting_UL[HT_IOT_PEER_MAX] = 
// UNKNOWN		REALTEK_90	REALTEK_92SE	BROADCOM		RALINK		ATHEROS		CISCO		MERU        MARVELL	92U_AP		SELF_AP(DownLink/Tx)
{ 0x5e4322, 		0xa44f, 		0x5e4322,		0x5ea32b,  		0x5ea422, 	0x5ea322,	0x3ea430,	0x5ea42b, 0x5ea44f,	0x5e4322,	0x5e4322};


static u4Byte edca_setting_DL[HT_IOT_PEER_MAX] = 
// UNKNOWN		REALTEK_90	REALTEK_92SE	BROADCOM		RALINK		ATHEROS		CISCO		MERU,       MARVELL	92U_AP		SELF_AP(UpLink/Rx)
{ 0xa44f, 		0x5ea44f, 	0x5e4322, 		0x5ea42b, 		0xa44f, 		0xa630, 		0x5ea630,	0x5ea42b, 0xa44f,		0xa42b,		0xa42b};

static u4Byte edca_setting_DL_GMode[HT_IOT_PEER_MAX] = 
// UNKNOWN		REALTEK_90	REALTEK_92SE	BROADCOM		RALINK		ATHEROS		CISCO		MERU,       MARVELL	92U_AP		SELF_AP
{ 0x4322, 		0xa44f, 		0x5e4322,		0xa42b, 			0x5e4322, 	0x4322, 		0xa42b,		0x5ea42b, 0xa44f,		0x5e4322,	0x5ea42b};
#endif


VOID
ODM_EdcaTurboInit(
	IN 	PVOID	 	pDM_VOID)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
#if ((DM_ODM_SUPPORT_TYPE == ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
	odm_EdcaParaInit(pDM_Odm);
#endif	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial VO PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_VO_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial VI PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_VI_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BE PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BE_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BK PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BK_PARAM)));

	
}	// ODM_InitEdcaTurbo

VOID
odm_EdcaTurboCheck(
	IN 	PVOID	 	pDM_VOID
	)
{
	// 
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//

	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("odm_EdcaTurboCheck========================>\n"));
	
	if(!(pDM_Odm->SupportAbility& ODM_MAC_EDCA_TURBO ))
		return;

	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
			odm_EdcaTurboCheckMP(pDM_Odm);
#endif
			break;

		case	ODM_CE:
#if(DM_ODM_SUPPORT_TYPE==ODM_CE)
			odm_EdcaTurboCheckCE(pDM_Odm);
#endif
			break;

		case	ODM_AP:
		case	ODM_ADSL:

#if ((DM_ODM_SUPPORT_TYPE == ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
		odm_IotEngine(pDM_Odm);
#endif
			break;	
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("<========================odm_EdcaTurboCheck\n"));

}	// odm_CheckEdcaTurbo

VOID
odm_EdcaTurboCheckCE(
	IN 	PVOID	 	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		       Adapter = pDM_Odm->Adapter;
	u32	EDCA_BE_UL = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_UL[pMgntInfo->IOTPeer];
	u32	EDCA_BE_DL = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_DL[pMgntInfo->IOTPeer];
	u32	ICType=pDM_Odm->SupportICType;
	u32	IOTPeer=0;
	u8	WirelessMode=0xFF;                   //invalid value
	u32 	trafficIndex;
	u32	edca_param;
	u64	cur_tx_bytes = 0;
	u64	cur_rx_bytes = 0;
	u8	bbtchange = _FALSE;
	u8	bBiasOnRx = _FALSE;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct dvobj_priv		*pdvobjpriv = adapter_to_dvobj(Adapter);
	struct recv_priv		*precvpriv = &(Adapter->recvpriv);
	struct registry_priv	*pregpriv = &Adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(Adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if(pDM_Odm->bLinked != _TRUE)
	{
		precvpriv->bIsAnyNonBEPkts = _FALSE;
		return;
	}

	if ((pregpriv->wifi_spec == 1) )//|| (pmlmeinfo->HT_enable == 0))
	{
		precvpriv->bIsAnyNonBEPkts = _FALSE;
		return;
	}

	if(pDM_Odm->pWirelessMode!=NULL)
		WirelessMode=*(pDM_Odm->pWirelessMode);

	IOTPeer = pmlmeinfo->assoc_AP_vendor;

	if (IOTPeer >=  HT_IOT_PEER_MAX)
	{
		precvpriv->bIsAnyNonBEPkts = _FALSE;
		return;
	}

	if(	(pDM_Odm->SupportICType == ODM_RTL8192C) ||
		(pDM_Odm->SupportICType == ODM_RTL8723A) ||
		(pDM_Odm->SupportICType == ODM_RTL8188E))
	{
		if((IOTPeer == HT_IOT_PEER_RALINK)||(IOTPeer == HT_IOT_PEER_ATHEROS))
			bBiasOnRx = _TRUE;
	}

	// Check if the status needs to be changed.
	if((bbtchange) || (!precvpriv->bIsAnyNonBEPkts) )
	{
		cur_tx_bytes = pdvobjpriv->traffic_stat.cur_tx_bytes;
		cur_rx_bytes = pdvobjpriv->traffic_stat.cur_rx_bytes;

		//traffic, TX or RX
		if(bBiasOnRx)
		{
			if (cur_tx_bytes > (cur_rx_bytes << 2))
			{ // Uplink TP is present.
				trafficIndex = UP_LINK; 
			}
			else
			{ // Balance TP is present.
				trafficIndex = DOWN_LINK;
			}
		}
		else
		{
			if (cur_rx_bytes > (cur_tx_bytes << 2))
			{ // Downlink TP is present.
				trafficIndex = DOWN_LINK;
			}
			else
			{ // Balance TP is present.
				trafficIndex = UP_LINK;
			}
		}

		//if ((pDM_Odm->DM_EDCA_Table.prv_traffic_idx != trafficIndex) || (!pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA))
		{
			if(ICType==ODM_RTL8192D)
			{      
				// Single PHY
				if(pDM_Odm->RFType==ODM_2T2R)
				{
					EDCA_BE_UL = 0x60a42b;    //0x5ea42b;
					EDCA_BE_DL = 0x60a42b;    //0x5ea42b;
				}
				else
				{
					EDCA_BE_UL = 0x6ea42b;
					EDCA_BE_DL = 0x6ea42b;
				}
			}
			else
			{
				if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE) {
					if((ICType==ODM_RTL8192C)&&(pDM_Odm->RFType==ODM_2T2R)) {
						EDCA_BE_UL = 0x60a42b;
						EDCA_BE_DL = 0x60a42b;
					}
					else
					{
						EDCA_BE_UL = 0x6ea42b;
						EDCA_BE_DL = 0x6ea42b;
					}
				}
			}
		
			//92D txop can't be set to 0x3e for cisco1250
			if((ICType!=ODM_RTL8192D) && (IOTPeer== HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
			{
				EDCA_BE_DL = edca_setting_DL[IOTPeer];
				EDCA_BE_UL = edca_setting_UL[IOTPeer];
			}
			//merge from 92s_92c_merge temp brunch v2445    20120215 
			else if((IOTPeer == HT_IOT_PEER_CISCO) &&((WirelessMode==ODM_WM_G)||(WirelessMode==(ODM_WM_B|ODM_WM_G))||(WirelessMode==ODM_WM_A)||(WirelessMode==ODM_WM_B)))
			{
				EDCA_BE_DL = edca_setting_DL_GMode[IOTPeer];
			}
			else if((IOTPeer== HT_IOT_PEER_AIRGO )&& ((WirelessMode==ODM_WM_G)||(WirelessMode==ODM_WM_A)))
			{
				EDCA_BE_DL = 0xa630;
			}
			else if(IOTPeer == HT_IOT_PEER_MARVELL)
			{
				EDCA_BE_DL = edca_setting_DL[IOTPeer];
				EDCA_BE_UL = edca_setting_UL[IOTPeer];
			}
			else if(IOTPeer == HT_IOT_PEER_ATHEROS)
			{
				// Set DL EDCA for Atheros peer to 0x3ea42b. Suggested by SD3 Wilson for ASUS TP issue. 
				EDCA_BE_DL = edca_setting_DL[IOTPeer];
			}

			if((ICType==ODM_RTL8812)||(ICType==ODM_RTL8821)||(ICType==ODM_RTL8192E))           //add 8812AU/8812AE
			{
				EDCA_BE_UL = 0x5ea42b;
				EDCA_BE_DL = 0x5ea42b;

				ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("8812A: EDCA_BE_UL=0x%x EDCA_BE_DL =0x%x",EDCA_BE_UL,EDCA_BE_DL));
			}

			if (trafficIndex == DOWN_LINK)
				edca_param = EDCA_BE_DL;
			else
				edca_param = EDCA_BE_UL;

			rtw_write32(Adapter, REG_EDCA_BE_PARAM, edca_param);

			pDM_Odm->DM_EDCA_Table.prv_traffic_idx = trafficIndex;
		}
		
		pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = _TRUE;
	}
	else
	{
		//
		// Turn Off EDCA turbo here.
		// Restore original EDCA according to the declaration of AP.
		//
		 if(pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA)
		{
			rtw_write32(Adapter, REG_EDCA_BE_PARAM, pHalData->AcParam_BE);
			pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = _FALSE;
		}
	}
}

//check if edca turbo is disabled
#if 0
	BOOLEAN
odm_IsEdcaTurboDisable(
	IN 	PVOID	 	pDM_VOID
)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	PADAPTER		       Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	u4Byte				IOTPeer=pMgntInfo->IOTPeer;

	if(pDM_Odm->bBtDisableEdcaTurbo)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("EdcaTurboDisable for BT!!\n"));
		return TRUE;
	}

	if((!(pDM_Odm->SupportAbility& ODM_MAC_EDCA_TURBO ))||
		(pDM_Odm->bWIFITest)||
		(IOTPeer>= HT_IOT_PEER_MAX))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("EdcaTurboDisable\n"));
		return TRUE;
	}


	// 1. We do not turn on EDCA turbo mode for some AP that has IOT issue
	// 2. User may disable EDCA Turbo mode with OID settings.
	if(pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_EDCA_TURBO){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("IOTAction:EdcaTurboDisable\n"));
		return	TRUE;
		}
		
	return	FALSE;
	

}
#endif

VOID
odm_EdcaChooseTrafficIdx( 
	IN 	PVOID	 	pDM_VOID,
	IN	u8Byte  			cur_tx_bytes,  
	IN	u8Byte  			cur_rx_bytes, 
	IN	BOOLEAN 		bBiasOnRx,
	OUT BOOLEAN 		*pbIsCurRDLState
	)
{	
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	
	if(bBiasOnRx)
	{
	  
		if(cur_tx_bytes>(cur_rx_bytes*4))
		{
			*pbIsCurRDLState=FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Uplink Traffic\n "));

		}
		else
		{
			*pbIsCurRDLState=TRUE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Balance Traffic\n"));

		}
	}
	else
	{
		if(cur_rx_bytes>(cur_tx_bytes*4))
		{
			*pbIsCurRDLState=TRUE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Downlink	Traffic\n"));

		}
		else
		{
			*pbIsCurRDLState=FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Balance Traffic\n"));
		}
	}

	return ;
}

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))

void odm_EdcaParaInit(
	IN 	PVOID	 	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	prtl8192cd_priv	priv		= pDM_Odm->priv;
	int   mode=priv->pmib->dot11BssType.net_work_type;
	
	static unsigned int slot_time, sifs_time;
	struct ParaRecord EDCA[4];

	 memset(EDCA, 0, 4*sizeof(struct ParaRecord));

	sifs_time = 10;
	slot_time = 20;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G))
		sifs_time = 16;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G| ODM_WM_G|ODM_WM_A))
		slot_time = 9;


#ifdef RTK_AC_SUPPORT //for 11ac logo,  edit aifs time for cca test cases
	if(AC_SIGMA_MODE != AC_SIGMA_NONE)
		sifs_time = 10;	
#endif


#if((defined(RTL_MANUAL_EDCA))&&(DM_ODM_SUPPORT_TYPE==ODM_AP))
	 if( priv->pmib->dot11QosEntry.ManualEDCA ) {
		 if( OPMODE & WIFI_AP_STATE )
			 memcpy(EDCA, priv->pmib->dot11QosEntry.AP_manualEDCA, 4*sizeof(struct ParaRecord));
		 else
			 memcpy(EDCA, priv->pmib->dot11QosEntry.STA_manualEDCA, 4*sizeof(struct ParaRecord));

		#ifdef WIFI_WMM
		if (QOS_ENABLE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[VI].TXOPlimit<< 16) | (EDCA[VI].ECWmax<< 12) | (EDCA[VI].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
		else
		#endif
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[BE].TXOPlimit<< 16) | (EDCA[BE].ECWmax<< 12) | (EDCA[BE].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));

	}else
	#endif //RTL_MANUAL_EDCA
	{

		 if(OPMODE & WIFI_AP_STATE)
		 {
		 	memcpy(EDCA, rtl_ap_EDCA, 2*sizeof(struct ParaRecord));

			if(mode & (ODM_WM_A|ODM_WM_G|ODM_WM_N24G|ODM_WM_N5G))
				memcpy(&EDCA[VI], &rtl_ap_EDCA[VI_AG], 2*sizeof(struct ParaRecord));
			else
				memcpy(&EDCA[VI], &rtl_ap_EDCA[VI], 2*sizeof(struct ParaRecord));
		 }
		 else
		 {
		 	memcpy(EDCA, rtl_sta_EDCA, 2*sizeof(struct ParaRecord));

			if(mode & (ODM_WM_A|ODM_WM_G|ODM_WM_N24G|ODM_WM_N5G))
				memcpy(&EDCA[VI], &rtl_sta_EDCA[VI_AG], 2*sizeof(struct ParaRecord));
			else
				memcpy(&EDCA[VI], &rtl_sta_EDCA[VI], 2*sizeof(struct ParaRecord));
		 }
		 
	#ifdef WIFI_WMM
		if (QOS_ENABLE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[VI].TXOPlimit<< 16) | (EDCA[VI].ECWmax<< 12) | (EDCA[VI].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
		else
	#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM,  (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM,  (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + 2* slot_time));
#endif
			

	}

	ODM_Write4Byte(pDM_Odm, ODM_EDCA_VO_PARAM, (EDCA[VO].TXOPlimit<< 16) | (EDCA[VO].ECWmax<< 12) | (EDCA[VO].ECWmin<< 8) | (sifs_time + EDCA[VO].AIFSN* slot_time));
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (EDCA[BE].TXOPlimit<< 16) | (EDCA[BE].ECWmax<< 12) | (EDCA[BE].ECWmin<< 8) | (sifs_time + EDCA[BE].AIFSN* slot_time));
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (EDCA[BK].TXOPlimit<< 16) | (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + EDCA[BK].AIFSN* slot_time));

#if defined(RTK_AC_SUPPORT) && defined(RTL_MANUAL_EDCA) //for 11ac logo,  make BK worse to seperate with BE.
	if((AC_SIGMA_MODE != AC_SIGMA_NONE) && (priv->pmib->dot11QosEntry.ManualEDCA))
	{
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (EDCA[BK].TXOPlimit<< 16) | (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | 0xa4 );
	}
#endif

//	ODM_Write1Byte(pDM_Odm,ACMHWCTRL, 0x00);

	priv->pshare->iot_mode_enable = 0;
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	if (priv->pshare->rf_ft_var.wifi_beq_iot)
		priv->pshare->iot_mode_VI_exist = 0;
	
	#ifdef WMM_VIBE_PRI
	priv->pshare->iot_mode_BE_exist = 0;
	#endif
	
#ifdef WMM_BEBK_PRI
	priv->pshare->iot_mode_BK_exist = 0;
#endif
	
	#ifdef LOW_TP_TXOP
	priv->pshare->BE_cwmax_enhance = 0;
	#endif

#elif (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
      priv->pshare->iot_mode_BE_exist = 0;   
#endif
	priv->pshare->iot_mode_VO_exist = 0;
}

BOOLEAN
ODM_ChooseIotMainSTA(
	IN 	PVOID	 	pDM_VOID,
	IN	PSTA_INFO_T		pstat
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	prtl8192cd_priv	priv = pDM_Odm->priv;
	BOOLEAN		bhighTP_found_pstat=FALSE;
	
	if ((GET_ROOT(priv)->up_time % 2) == 0) {
		unsigned int tx_2s_avg = 0;
		unsigned int rx_2s_avg = 0;
		int i=0, aggReady=0;
		unsigned long total_sum = (priv->pshare->current_tx_bytes+priv->pshare->current_rx_bytes);
		int assoc_num = GET_ROOT(priv)->assoc_num;
#ifdef MBSSID
		if (GET_ROOT(priv)->pmib->miscEntry.vap_enable){
			for (i=0; i<RTL8192CD_NUM_VWLAN; ++i)
				assoc_num += GET_ROOT(priv)->pvap_priv[i]-> assoc_num;
		}
#endif	
#ifdef UNIVERSAL_REPEATER
		if (IS_DRV_OPEN(GET_VXD_PRIV(GET_ROOT(priv))))
			assoc_num += GET_VXD_PRIV(GET_ROOT(priv))-> assoc_num;
#endif
#ifdef WDS
		 if(GET_ROOT(priv)->pmib->dot11WdsInfo.wdsEnabled)
			assoc_num ++;
#endif


		pstat->current_tx_bytes += pstat->tx_byte_cnt;
		pstat->current_rx_bytes += pstat->rx_byte_cnt;

		if (total_sum != 0) {
			if (total_sum <= 1000000) {
			tx_2s_avg = (unsigned int)((pstat->current_tx_bytes*100) / total_sum);
			rx_2s_avg = (unsigned int)((pstat->current_rx_bytes*100) / total_sum);
			} else {
				tx_2s_avg = (unsigned int)(pstat->current_tx_bytes / (total_sum / 100));
				rx_2s_avg = (unsigned int)(pstat->current_rx_bytes / (total_sum / 100));
			}

		}

#if(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (pstat->ht_cap_len) {
			if ((tx_2s_avg + rx_2s_avg) >=25 ) {//50//

					priv->pshare->highTP_found_pstat = pstat;
					bhighTP_found_pstat=TRUE;
   				}
			}
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
		for(i=0; i<8; i++)
			aggReady += (pstat->ADDBA_ready[i]);

		if ((pstat->ht_cap_len && (
#ifdef	SUPPORT_TX_AMSDU			
			AMSDU_ENABLE || 
#endif			
			aggReady)) || (pstat->IOTPeer==HT_IOT_PEER_INTEL))
		{
			if ((assoc_num==1) || (tx_2s_avg + rx_2s_avg >= 25)) {
				priv->pshare->highTP_found_pstat = pstat;
			}
			
		#ifdef CLIENT_MODE
			if (OPMODE & WIFI_STATION_STATE) {
				if ((tx_2s_avg + rx_2s_avg) >= 20)
					priv->pshare->highTP_found_pstat = pstat;
		}
		#endif				
	}
#endif
	} 
	else {
		pstat->current_tx_bytes = pstat->tx_byte_cnt;
		pstat->current_rx_bytes = pstat->rx_byte_cnt;
	}

	return bhighTP_found_pstat;
}


#ifdef WIFI_WMM
VOID
ODM_IotEdcaSwitch(
	IN 	PVOID	 	pDM_VOID,
	IN	unsigned char		enable
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	prtl8192cd_priv	priv	= pDM_Odm->priv;
	int   mode=priv->pmib->dot11BssType.net_work_type;
	unsigned int slot_time = 20, sifs_time = 10, BE_TXOP = 47, VI_TXOP = 94;
	unsigned int vi_cw_max = 4, vi_cw_min = 3, vi_aifs;
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	u32 be_edca, vi_edca;
	u16 disable_cfe;
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
	if (!(!priv->pmib->dot11OperationEntry.wifi_specific ||
		((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
	#ifdef CLIENT_MODE
		|| ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
	#endif
		))
		return;
#endif

#ifdef RTK_AC_SUPPORT //for 11ac logo, do not dynamic switch edca 
	if(AC_SIGMA_MODE != AC_SIGMA_NONE)
		return;
#endif

	if ((mode & (ODM_WM_N24G|ODM_WM_N5G)) && (priv->pshare->ht_sta_num
	#ifdef WDS
		|| ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11WdsInfo.wdsEnabled && priv->pmib->dot11WdsInfo.wdsNum)
	#endif
		))
		sifs_time = 16;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G|ODM_WM_G|ODM_WM_A)) {
		slot_time = 9;
	} 
	else
	{
		BE_TXOP = 94;
		VI_TXOP = 188;
	}

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	vi_edca = -1;
	disable_cfe = -1;
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
	if (priv->pshare->iot_mode_VO_exist) {
		// to separate AC_VI and AC_BE to avoid using the same EDCA settings
		if (priv->pshare->iot_mode_BE_exist) {
			vi_cw_max = 5;
			vi_cw_min = 3;
		} else {
			vi_cw_max = 6;
			vi_cw_min = 4;
		}
	}
	vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	vi_edca = ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs;
#else
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs);
#endif
	
#elif (DM_ODM_SUPPORT_TYPE==ODM_AP)
	if ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11OperationEntry.wifi_specific) {
		if (priv->pshare->iot_mode_VO_exist) {
	#ifdef WMM_VIBE_PRI
			if (priv->pshare->iot_mode_BE_exist) 
			{
				vi_cw_max = 5;
				vi_cw_min = 3;
				vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);
			}
			else 
	#endif
			{
			vi_cw_max = 6;
			vi_cw_min = 4;
			vi_aifs = 0x2b;
			}
		} 
		else {
			vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);
		}

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
		vi_edca = ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)
			| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs;
#else
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)
			| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs);
#endif

	#ifdef WMM_BEBK_PRI
	#ifdef CONFIG_RTL_88E_SUPPORT
		if ((GET_CHIP_VER(priv) == VERSION_8188E) && priv->pshare->iot_mode_BK_exist) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (10 << 12) | (6 << 8) | 0x4f;
#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (10 << 12) | (6 << 8) | 0x4f);
#endif
		}
	#endif		
	#endif
#if defined(CONFIG_WLAN_HAL_8881A) 
		if (GET_CHIP_VER(priv) == VERSION_8881A) 
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM,  0xa64f);
#endif		
	}
#endif



#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
 	if (priv->pshare->rf_ft_var.wifi_beq_iot && priv->pshare->iot_mode_VI_exist) {
#if defined(CONFIG_RTL_88E_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
		if (GET_CHIP_VER(priv) == VERSION_8188E || GET_CHIP_VER(priv) == VERSION_8812E) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (10 << 12) | (6 << 8) | 0x4f;
#else
		  	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (10 << 12) | (6 << 8) | 0x4f);
#endif
		}
		else
#endif
		{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (10 << 12) | (4 << 8) | 0x4f;
#else
	  	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (10 << 12) | (4 << 8) | 0x4f);
#endif
		}
	} else if(!enable)
#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)      
	if(!enable)                                 //if iot is disable ,maintain original BEQ PARAM
#endif
	{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
		be_edca = (((OPMODE & WIFI_AP_STATE)?6:10) << 12) | (4 << 8)
			| (sifs_time + 3 * slot_time);
		disable_cfe = 1;
#else
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (((OPMODE & WIFI_AP_STATE)?6:10) << 12) | (4 << 8)
			| (sifs_time + 3 * slot_time));
#endif
#ifdef CONFIG_PCI_HCI
//		ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));
#endif
	}
	else
	{
		int txop;
		unsigned int cw_max;
#ifdef LOW_TP_TXOP
		unsigned int txop_close;
#endif
		
	#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP))
			cw_max = ((priv->pshare->BE_cwmax_enhance) ? 10 : 6);
			txop_close = ((priv->pshare->rf_ft_var.low_tp_txop && priv->pshare->rf_ft_var.low_tp_txop_close) ? 1 : 0);

			if(priv->pshare->txop_enlarge == 0xe)   //if intel case
				txop = (txop_close ? 0 : (BE_TXOP*2));
			else                                                        //if other case
				txop = (txop_close ? 0: (BE_TXOP*priv->pshare->txop_enlarge));
	#else
			cw_max=6;
			if((priv->pshare->txop_enlarge==0xe)||(priv->pshare->txop_enlarge==0xd))
				txop=BE_TXOP*2;
			else
				txop=BE_TXOP*priv->pshare->txop_enlarge;

	#endif
                           
		if (priv->pshare->ht_sta_num
	#ifdef WDS
			|| ((OPMODE & WIFI_AP_STATE) && (mode & (ODM_WM_N24G|ODM_WM_N5G)) &&
			priv->pmib->dot11WdsInfo.wdsEnabled && priv->pmib->dot11WdsInfo.wdsNum)
	#endif
			) 
			{

			if (priv->pshare->txop_enlarge == 0xe) {
				// is intel client, use a different edca value
				//ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop<< 16) | (cw_max<< 12) | (4 << 8) | 0x1f);
				if (pDM_Odm->RFType==ODM_1T1R) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (5 << 12) | (3 << 8) | 0x1f;
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (5 << 12) | (3 << 8) | 0x1f);
#endif
				}
				else {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (8 << 12) | (5 << 8) | 0x1f;
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (8 << 12) | (5 << 8) | 0x1f);
#endif
				}
				
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				disable_cfe = 0;
#endif
#ifdef CONFIG_PCI_HCI
//				ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) & ~(DIS_TXOP_CFE));
#endif
				priv->pshare->txop_enlarge = 2;
			} 
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	#ifndef LOW_TP_TXOP
			 else if (priv->pshare->txop_enlarge == 0xd) {
				// is intel ralink, use a different edca value
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				be_edca = (txop << 16) | (6 << 12) | (5 << 8) | 0x2b;
#else
				ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (6 << 12) | (5 << 8) | 0x2b);
#endif
				priv->pshare->txop_enlarge = 2;
			} 
	#endif
#endif
			else 
			{
//				if (txop == 0) {
//#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
//					disable_cfe = 1;
//#endif
//#ifdef CONFIG_PCI_HCI
//					ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));
//#endif
//				}
					
				if (pDM_Odm->RFType==ODM_2T2R) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time);
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
#endif
				}
				else
				#if(DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP)
				{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) |
						(((priv->pshare->BE_cwmax_enhance) ? 10 : 5) << 12) | (3 << 8) | (sifs_time + 2 * slot_time);
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(((priv->pshare->BE_cwmax_enhance) ? 10 : 5) << 12) | (3 << 8) | (sifs_time + 2 * slot_time));
#endif
				}
				#else
				{
					PSTA_INFO_T		pstat = priv->pshare->highTP_found_pstat;
					if ((GET_CHIP_VER(priv)==VERSION_8881A) && pstat && (pstat->IOTPeer == HT_IOT_PEER_HTC))
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, 0x642b);
					else {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (5 << 12) | (3 << 8) | (sifs_time + 2 * slot_time);
				#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(5 << 12) | (3 << 8) | (sifs_time + 2 * slot_time));
#endif
					}
				}
				#endif
			}
		}
              else 
              {
 #if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP))
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			 be_edca = (BE_TXOP << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time);
#else
			 ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (BE_TXOP << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
#endif
 #else
 		#if defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8197DL) || defined(CONFIG_RTL_8196E) || (defined(CONFIG_RTL_8197D) && !defined(CONFIG_PORT0_EXT_GIGA))
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (BE_TXOP*2 << 16) | (cw_max << 12) | (5 << 8) | (sifs_time + 3 * slot_time);
 #else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (BE_TXOP*2 << 16) | (cw_max << 12) | (5 << 8) | (sifs_time + 3 * slot_time));
#endif
		#else
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (BE_TXOP*2 << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time);
		#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (BE_TXOP*2 << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
		#endif
		#endif
/*		
		if (priv->pshare->txop_enlarge == 0xe) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			disable_cfe = 0;
#endif
	#ifdef CONFIG_PCI_HCI
			ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) & ~(DIS_TXOP_CFE));
	#endif
		} else {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			disable_cfe = 1;
#endif
	#ifdef CONFIG_PCI_HCI
			ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));
	#endif
		}
*/			
 #endif
              }

	}
	
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	notify_IOT_EDCA_switch(priv, be_edca, vi_edca, disable_cfe);
#endif
}
#endif

VOID 
odm_IotEngine(
	IN 	PVOID	 	pDM_VOID
	)
{
	PDM_ODM_T		pDM_Odm = (PDM_ODM_T)pDM_VOID;
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	PSTA_INFO_T pstat = NULL;
	u4Byte i;
	
#ifdef WIFI_WMM
	unsigned int switch_turbo = 0, avg_tp;
#endif	
////////////////////////////////////////////////////////
//  if EDCA Turbo function is not supported or Manual EDCA Setting
//  then return
////////////////////////////////////////////////////////
	if(!(pDM_Odm->SupportAbility&ODM_MAC_EDCA_TURBO)){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO NOT SUPPORTED\n"));
		return;
	}
	
#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&& defined(RTL_MANUAL_EDCA) && defined(WIFI_WMM))
	if(priv->pmib->dot11QosEntry.ManualEDCA){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO OFF: MANUAL SETTING\n"));
		return ;
	}
#endif 

#if !(DM_ODM_SUPPORT_TYPE &ODM_AP)
 //////////////////////////////////////////////////////
 //find high TP STA every 2s
//////////////////////////////////////////////////////
	if ((GET_ROOT(priv)->up_time % 2) == 0) 
		priv->pshare->highTP_found_pstat==NULL;

#if 0
	phead = &priv->asoc_list;
	plist = phead->next;
	while(plist != phead)	{
		pstat = list_entry(plist, struct stat_info, asoc_list);

		if(ODM_ChooseIotMainSTA(pDM_Odm, pstat));              //find the correct station
			break;
		if (plist == plist->next)                                          //the last plist 
			break;
		plist = plist->next;
	};
#endif

	//find highTP STA
	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pstat = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pstat) && (ODM_ChooseIotMainSTA(pDM_Odm, pstat)))	 //find the correct station
				break;
	}

 //////////////////////////////////////////////////////
 //if highTP STA is not found, then return
 //////////////////////////////////////////////////////
	if(priv->pshare->highTP_found_pstat==NULL)	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO OFF: NO HT STA FOUND\n"));
		return;
	}
#endif

	pstat=priv->pshare->highTP_found_pstat;
	if(pstat) {
		if((pstat->tx_avarage + pstat->rx_avarage) < (1<<17))	// 1M bps
			pstat = NULL;
	}

#ifdef WIFI_WMM
	if (QOS_ENABLE) {
		if (!priv->pmib->dot11OperationEntry.wifi_specific 
		#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
			||((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
		#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
			|| (priv->pmib->dot11OperationEntry.wifi_specific == 2)
		#endif
			) {
			if (priv->pshare->iot_mode_enable &&
				((priv->pshare->phw->VO_pkt_count > 50) ||
				 (priv->pshare->phw->VI_pkt_count > 50) ||
				 (priv->pshare->phw->BK_pkt_count > 50))) {
				priv->pshare->iot_mode_enable = 0;
				switch_turbo++;
#ifdef CONFIG_WLAN_HAL_8881A
				if (GET_CHIP_VER(priv) == VERSION_8881A) {
					RTL_W32(0x460, 0x03086666);
				}
#endif //CONFIG_WLAN_HAL_8881A
			} else if ((!priv->pshare->iot_mode_enable) &&
				((priv->pshare->phw->VO_pkt_count < 50) &&
				 (priv->pshare->phw->VI_pkt_count < 50) &&
				 (priv->pshare->phw->BK_pkt_count < 50))) {
				priv->pshare->iot_mode_enable++;
				switch_turbo++;
//#ifdef CONFIG_WLAN_HAL_8881A
#if 0
				if (GET_CHIP_VER(priv) == VERSION_8881A) {
					if (get_bonding_type_8881A()==BOND_8881AB) {
						RTL_W32(0x460, 0x03086666);
					}
					else {
						RTL_W32(0x460, 0x0320ffff);
					}
				}
#endif //CONFIG_WLAN_HAL_8881A
			}
		}


		#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11OperationEntry.wifi_specific)
		#elif (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (priv->pmib->dot11OperationEntry.wifi_specific) 
		#endif
		{
			if (!priv->pshare->iot_mode_VO_exist && (priv->pshare->phw->VO_pkt_count > 50)) {
				priv->pshare->iot_mode_VO_exist++;
				switch_turbo++;
			} else if (priv->pshare->iot_mode_VO_exist && (priv->pshare->phw->VO_pkt_count < 50)) {
				priv->pshare->iot_mode_VO_exist = 0;
				switch_turbo++;
			}
#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_VIBE_PRI)))
			if (priv->pshare->iot_mode_VO_exist) {
				//printk("[%s %d] BE_pkt_count=%d\n", __FUNCTION__, __LINE__, priv->pshare->phw->BE_pkt_count);
				if (!priv->pshare->iot_mode_BE_exist && (priv->pshare->phw->BE_pkt_count > 250)) {
					priv->pshare->iot_mode_BE_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_BE_exist && (priv->pshare->phw->BE_pkt_count < 250)) {
					priv->pshare->iot_mode_BE_exist = 0;
					switch_turbo++;
				}
			}
#endif

#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_BEBK_PRI)))
			if (priv->pshare->phw->BE_pkt_count) {
				//printk("[%s %d] BK_pkt_count=%d\n", __FUNCTION__, __LINE__, priv->pshare->phw->BK_pkt_count);
				if (!priv->pshare->iot_mode_BK_exist && (priv->pshare->phw->BK_pkt_count > 250)) {
					priv->pshare->iot_mode_BK_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_BK_exist && (priv->pshare->phw->BK_pkt_count < 250)) {
					priv->pshare->iot_mode_BK_exist = 0;
					switch_turbo++;
				}
			}
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
			if (priv->pshare->rf_ft_var.wifi_beq_iot) 
			{
				if (!priv->pshare->iot_mode_VI_exist && (priv->pshare->phw->VI_rx_pkt_count > 50)) {
					priv->pshare->iot_mode_VI_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_VI_exist && (priv->pshare->phw->VI_rx_pkt_count < 50)) {
					priv->pshare->iot_mode_VI_exist = 0;
					switch_turbo++;
				}
			}
#endif

		}
		else if (!pstat || pstat->rssi < priv->pshare->rf_ft_var.txop_enlarge_lower) {
		   if (priv->pshare->txop_enlarge) {
			   priv->pshare->txop_enlarge = 0;
			   if (priv->pshare->iot_mode_enable)
					switch_turbo++;
				}
         	}

#if(defined(CLIENT_MODE) && (DM_ODM_SUPPORT_TYPE==ODM_AP))
        if ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
        {
            if (priv->pshare->iot_mode_enable &&
                (((priv->pshare->phw->VO_pkt_count > 50) ||
                 (priv->pshare->phw->VI_pkt_count > 50) ||
                 (priv->pshare->phw->BK_pkt_count > 50)) ||
                 (pstat && (!pstat->ADDBA_ready[0]) & (!pstat->ADDBA_ready[3]))))
            {
                priv->pshare->iot_mode_enable = 0;
                switch_turbo++;
            }
            else if ((!priv->pshare->iot_mode_enable) &&
                (((priv->pshare->phw->VO_pkt_count < 50) &&
                 (priv->pshare->phw->VI_pkt_count < 50) &&
                 (priv->pshare->phw->BK_pkt_count < 50)) &&
                 (pstat && (pstat->ADDBA_ready[0] | pstat->ADDBA_ready[3]))))
            {
                priv->pshare->iot_mode_enable++;
                switch_turbo++;
            }
        }
#endif

		priv->pshare->phw->VO_pkt_count = 0;
		priv->pshare->phw->VI_pkt_count = 0;
		priv->pshare->phw->BK_pkt_count = 0;

	#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_VIBE_PRI)))
		priv->pshare->phw->BE_pkt_count = 0;
	#endif
		
	#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if (priv->pshare->rf_ft_var.wifi_beq_iot)
			priv->pshare->phw->VI_rx_pkt_count = 0;
		#endif

	}
#endif

	if ((priv->up_time % 2) == 0) {
		/*
		 * decide EDCA content for different chip vendor
		 */
#ifdef WIFI_WMM
	#if(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (QOS_ENABLE && (!priv->pmib->dot11OperationEntry.wifi_specific || (priv->pmib->dot11OperationEntry.wifi_specific == 2)
	
	#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if (QOS_ENABLE && (!priv->pmib->dot11OperationEntry.wifi_specific || 
			((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#ifdef CLIENT_MODE
            || ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#endif
	#endif
		))
	
		{

			if (pstat && pstat->rssi >= priv->pshare->rf_ft_var.txop_enlarge_upper) {
#ifdef LOW_TP_TXOP
				if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
				{
					if (priv->pshare->txop_enlarge != 0xe)
					{
						priv->pshare->txop_enlarge = 0xe;

						if (priv->pshare->iot_mode_enable)
							switch_turbo++;
					}
				} 
				else if (priv->pshare->txop_enlarge != 2) 
				{
					priv->pshare->txop_enlarge = 2;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#else
				if (priv->pshare->txop_enlarge != 2)
				{
					if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
						priv->pshare->txop_enlarge = 0xe;						
					else if (pstat->IOTPeer==HT_IOT_PEER_RALINK)
						priv->pshare->txop_enlarge = 0xd;						
					else if (pstat->IOTPeer==HT_IOT_PEER_HTC)
						priv->pshare->txop_enlarge = 0;		
					else
						priv->pshare->txop_enlarge = 2;

					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#endif
			}
			else if (!pstat || pstat->rssi < priv->pshare->rf_ft_var.txop_enlarge_lower) 
			{
				if (priv->pshare->txop_enlarge) {
					priv->pshare->txop_enlarge = 0;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
			}

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&( defined LOW_TP_TXOP))
			// for Intel IOT, need to enlarge CW MAX from 6 to 10
			if (pstat && pstat->IOTPeer==HT_IOT_PEER_INTEL && (((pstat->tx_avarage+pstat->rx_avarage)>>10) < 
					priv->pshare->rf_ft_var.cwmax_enhance_thd)) 
			{
				if (!priv->pshare->BE_cwmax_enhance && priv->pshare->iot_mode_enable)
				{
					priv->pshare->BE_cwmax_enhance = 1;
					switch_turbo++;
				}
			} else {
				if (priv->pshare->BE_cwmax_enhance) {
					priv->pshare->BE_cwmax_enhance = 0;
					switch_turbo++;
				}
			}
#endif
		}
#endif
		priv->pshare->current_tx_bytes = 0;
		priv->pshare->current_rx_bytes = 0;
	}else {
		if ((GET_CHIP_VER(priv) == VERSION_8881A)||(GET_CHIP_VER(priv) == VERSION_8192E)|| (GET_CHIP_VER(priv) == VERSION_8188E) ){
			unsigned int uldl_tp = (priv->pshare->current_tx_bytes+priv->pshare->current_rx_bytes)>>17;
			if((uldl_tp > 40) && (priv->pshare->agg_to!= 1)) {
				RTL_W8(0x462, 0x08);
				priv->pshare->agg_to = 1;
			} else if((uldl_tp < 35) && (priv->pshare->agg_to !=0)) {
				RTL_W8(0x462, 0x02);
				priv->pshare->agg_to = 0;
			} 
		}
	}
	
#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&& defined( SW_TX_QUEUE))
	if(AMPDU_ENABLE) {
#ifdef TX_EARLY_MODE
		if (GET_TX_EARLY_MODE) {
			if (!GET_EM_SWQ_ENABLE &&
				((priv->assoc_num > 1) ||
				(pstat && pstat->IOTPeer != HT_IOT_PEER_UNKNOWN))) {
				if ((priv->pshare->em_tx_byte_cnt >> 17) > EM_TP_UP_BOUND) 
					priv->pshare->reach_tx_limit_cnt++;				
				else					
					priv->pshare->reach_tx_limit_cnt = 0;	

				if (priv->pshare->txop_enlarge && priv->pshare->reach_tx_limit_cnt) { //>= WAIT_TP_TIME//
					GET_EM_SWQ_ENABLE = 1;			
					priv->pshare->reach_tx_limit_cnt = 0;

					if (pstat->IOTPeer == HT_IOT_PEER_INTEL)
						MAX_EM_QUE_NUM = 12;
					else if (pstat->IOTPeer == HT_IOT_PEER_RALINK)
						MAX_EM_QUE_NUM = 10;
					
					enable_em(priv);			
				}
			}
			else if (GET_EM_SWQ_ENABLE) {
				if ((priv->pshare->em_tx_byte_cnt >> 17) < EM_TP_LOW_BOUND)
					priv->pshare->reach_tx_limit_cnt++;				
				else					
					priv->pshare->reach_tx_limit_cnt = 0;	

				if (!priv->pshare->txop_enlarge || priv->pshare->reach_tx_limit_cnt >= WAIT_TP_TIME) {
					GET_EM_SWQ_ENABLE = 0;
					priv->pshare->reach_tx_limit_cnt = 0;
					disable_em(priv);
				}
			}
		}
#endif

#if defined(CONFIG_WLAN_HAL_8881A) || defined(CONFIG_WLAN_HAL_8192EE) || defined(CONFIG_RTL_8812_SUPPORT) || defined(CONFIG_WLAN_HAL_8814AE)
	if (pDM_Odm->SupportICType == ODM_RTL8881A || pDM_Odm->SupportICType == ODM_RTL8192E || pDM_Odm->SupportICType == ODM_RTL8812 || pDM_Odm->SupportICType == ODM_RTL8814A) {	
		if (priv->assoc_num > 9)
   	{
       	if (priv->swq_txmac_chg >= priv->pshare->rf_ft_var.swq_en_highthd){
				if ((priv->swq_decision == 0)){
				switch_turbo++;
				if (priv->pshare->txop_enlarge == 0)
					priv->pshare->txop_enlarge = 2;
					priv->swq_decision = 1;
				}
			else
			{
				if ((switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))
				{
					priv->pshare->txop_enlarge = 2;
					switch_turbo--;
				}
			}
		}
		else if(priv->swq_txmac_chg <= priv->pshare->rf_ft_var.swq_dis_lowthd){
				priv->swq_decision = 0;
		}
			else if ((priv->swq_decision == 1) && (switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))        {
			priv->pshare->txop_enlarge = 2;
			switch_turbo--;
		}
	    	} else {
			priv->swq_decision = 0;
    }
	} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
		{	
		if (priv->assoc_num > 1)
   	{
       	if (priv->swq_txmac_chg >= priv->pshare->rf_ft_var.swq_en_highthd){
				if ((priv->swq_decision == 0)){
				switch_turbo++;
				if (priv->pshare->txop_enlarge == 0)
					priv->pshare->txop_enlarge = 2;
					priv->swq_decision = 1;
				}
			else
			{
				if ((switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))
				{
					priv->pshare->txop_enlarge = 2;
					switch_turbo--;
				}
			}
		}
		else if(priv->swq_txmac_chg <= priv->pshare->rf_ft_var.swq_dis_lowthd){
				priv->swq_decision = 0;
		}
			else if ((priv->swq_decision == 1) && (switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))        {
			priv->pshare->txop_enlarge = 2;
			switch_turbo--;
		}
    }
	//#if (defined CONFIG_RTL_819XD))
		else if (priv->assoc_num == 1 && (priv->up_time % 2 == 0)
#if (DM_ODM_SUPPORT_TYPE==ODM_AP) && defined(TX_EARLY_MODE)
			&& (!GET_TX_EARLY_MODE || !GET_EM_SWQ_ENABLE) 
#endif
		) {
			if ((pstat) && (pstat->ADDBA_ready[0] | pstat->ADDBA_ready[3])) {
				//int en_thd = 14417920>>(priv->up_time % 2);
				avg_tp = (pstat->current_tx_bytes >> 17);

				//if ((priv->swq_decision == 0) && (pstat->current_tx_bytes > en_thd) && (pstat->current_rx_bytes > en_thd) )  { //50Mbps
				if ((priv->swq_decision == 0) && (avg_tp  > TP_HIGH_WATER_MARK)) { //55Mbps
					//printk("[%s:%d] swq_decision=1 current_tp: %d Mbps\n", __FUNCTION__, __LINE__, avg_tp);
					priv->swq_decision = 1;
				}
				//else if ((priv->swq_decision == 1) && ((pstat->tx_avarage < 4587520) || (pstat->rx_avarage < 4587520))) { //35Mbps
				else if ((priv->swq_decision == 1) && (avg_tp < TP_LOW_WATER_MARK)) { //35Mbps
					//printk("[%s:%d] swq_decision=0 current_tp: %d Mbps\n", __FUNCTION__, __LINE__, avg_tp);
					priv->swq_decision = 0;
				}
			} else {
				priv->swq_decision = 0;
            }
            }
        }
		if( (priv->swq_decision == 1) 
#if (DM_ODM_SUPPORT_TYPE==ODM_AP) && defined(TX_EARLY_MODE)		
		|| (GET_EM_SWQ_ENABLE == 1) 
#endif		
		) {
			priv->swq_en = 1;
			priv->swqen_keeptime = priv->up_time;
		} else {
            priv->swq_en = 0;
			priv->swqen_keeptime = 0;
        }
    }
#endif

#ifdef WIFI_WMM
#ifdef LOW_TP_TXOP
	if ((!priv->pmib->dot11OperationEntry.wifi_specific || (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		&& QOS_ENABLE) {
		if (switch_turbo || priv->pshare->rf_ft_var.low_tp_txop) {
			unsigned int thd_tp;
			unsigned char under_thd;
			unsigned int curr_tp;

			if (priv->pmib->dot11BssType.net_work_type & (ODM_WM_N24G|ODM_WM_N5G| ODM_WM_G))
			{
				// Determine the upper bound throughput threshold.
				if (priv->pmib->dot11BssType.net_work_type & (ODM_WM_N24G|ODM_WM_N5G)) {
					if (priv->assoc_num && priv->assoc_num != priv->pshare->ht_sta_num)
						thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_g;
					else
						thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_n;
				}
				else
					thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_g;

				// Determine to close txop.
#if defined(UNIVERSAL_REPEATER) || defined(MBSSID)
				if(IS_STA_VALID(pstat)) 
				{	
					struct rtl8192cd_priv *tmppriv;
					struct aid_obj *aidarray;	
					aidarray = container_of(pstat, struct aid_obj, station);
					tmppriv = aidarray->priv;

					curr_tp = (unsigned int)(tmppriv->ext_stats.tx_avarage>>17) + (unsigned int)(tmppriv->ext_stats.rx_avarage>>17);
				} 
				else 
#endif
				curr_tp = (unsigned int)(priv->ext_stats.tx_avarage>>17) + (unsigned int)(priv->ext_stats.rx_avarage>>17);
				if (curr_tp <= thd_tp && curr_tp >= priv->pshare->rf_ft_var.low_tp_txop_thd_low)
					under_thd = 1;
				else
					under_thd = 0;
			}
			else
			{
				under_thd = 0;
			}

			if (switch_turbo) 
			{
				priv->pshare->rf_ft_var.low_tp_txop_close = under_thd;
				priv->pshare->rf_ft_var.low_tp_txop_count = 0;
			}
			else if (priv->pshare->iot_mode_enable && (priv->pshare->rf_ft_var.low_tp_txop_close != under_thd)) {
				priv->pshare->rf_ft_var.low_tp_txop_count++;
				if (priv->pshare->rf_ft_var.low_tp_txop_close) {
					priv->pshare->rf_ft_var.low_tp_txop_count = priv->pshare->rf_ft_var.low_tp_txop_delay;
				}
				if (priv->pshare->rf_ft_var.low_tp_txop_count ==priv->pshare->rf_ft_var.low_tp_txop_delay) 

				{					
					priv->pshare->rf_ft_var.low_tp_txop_count = 0;
					priv->pshare->rf_ft_var.low_tp_txop_close = under_thd;
					switch_turbo++;
				}
			} 
			else 
			{
				priv->pshare->rf_ft_var.low_tp_txop_count = 0;
			}
		}
	}
#endif		

#ifdef WMM_DSCP_C42
	if (switch_turbo) {
		if (!priv->pshare->iot_mode_enable && !priv->pshare->aggrmax_change) {
			RTL_W16(0x4ca, 0x0404);
			priv->pshare->aggrmax_change = 1;
		}
		else if (priv->pshare->iot_mode_enable && priv->pshare->aggrmax_change) {
			RTL_W16(0x4ca, priv->pshare->aggrmax_bak);
			priv->pshare->aggrmax_change = 0;
		}
	} 
#endif
#ifdef TX_EARLY_MODE
		unsigned int em_tp = ((priv->ext_stats.tx_avarage>>17) + (priv->ext_stats.rx_avarage>>17));
		if (em_tp > 80)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (0x5e << 16) | (4 << 12) | (3 << 8) | 0x19);
		else //if (em_tp < 75)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (0x5e << 16) | (6 << 12) | (5 << 8) | 0x2b);
#endif
	if (switch_turbo)
		ODM_IotEdcaSwitch( pDM_Odm, priv->pshare->iot_mode_enable );
#endif
}
#endif
