/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

#include "AEConfig.h"

#ifdef AE_OS_WIN
#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "A.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"

enum {
	PIXELSORT_INPUT = 0,
	PIXELSORT_BRIGHTNESS,
  PIXELSORT_X,
  PIXELSORT_Y,
	PIXELSORT_NUM_PARAMS
};

#include "AEPixelSort.h"

extern "C" DllExport PF_Err EntryPointFunc (PF_Cmd cmd,PF_InData *in_data,PF_OutData *out_data,PF_ParamDef *params[],PF_LayerDef *output);

static PF_Err About(PF_InData	*in_data,PF_OutData	*out_data,PF_ParamDef *params[],PF_LayerDef	*output) {
	PF_SPRINTF(out_data->return_msg,"PixelSort, v1.0\rKim Asendorf's PixelSort for After Effects");

	return PF_Err_NONE;
}

static PF_Err GlobalSetup(PF_InData *in_data,PF_OutData	*out_data,PF_ParamDef *params[],PF_LayerDef	*output) {
	
  PF_Err err = PF_Err_NONE;
 
	out_data->my_version = PF_VERSION(2,0,0,PF_Stage_DEVELOP,1);
    
  out_data->out_flags = PF_OutFlag_I_EXPAND_BUFFER|PF_OutFlag_I_HAVE_EXTERNAL_DEPENDENCIES;
  out_data->out_flags2 =  PF_OutFlag2_NONE;

  in_data->sequence_data = out_data->sequence_data = nullptr;
  
	return err;
}

static PF_Err ParamsSetup(PF_InData	*in_data,PF_OutData *out_data,PF_ParamDef *params[],PF_LayerDef *output) {
  
	PF_Err err = PF_Err_NONE;
	PF_ParamDef def;
	
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Brightness Mode","",FALSE,0,PIXELSORT_BRIGHTNESS);
  
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("X",0,255,0,255,0,PIXELSORT_X);
  
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("Y",0,255,0,255,0,PIXELSORT_Y);

	out_data->num_params = PIXELSORT_NUM_PARAMS;

	return err;
}

static PF_Err SequenceSetup(PF_InData *in_data,PF_OutData *out_data,PF_ParamDef	*params[],PF_LayerDef *output) {
  
	return PF_Err_NONE;  
}

static PF_Err SequenceSetdown(PF_InData *in_data,PF_OutData *out_data,PF_ParamDef *params[],PF_LayerDef *output) {

  return PF_Err_NONE;
}

static PF_Err SequenceResetup(PF_InData	*in_data,PF_OutData *out_data,PF_ParamDef *params[],PF_LayerDef *output) {
  
	return PF_Err_NONE;
}

static PF_Err Render(PF_InData *in_data,PF_OutData *out_data, PF_ParamDef *params[],PF_LayerDef	*output) {

  AEPixelSort::render(&params[0]->u.ld,output,params);
  
  return PF_Err_NONE;
}

DllExport	PF_Err EntryPointFunc(PF_Cmd cmd,PF_InData *in_data,PF_OutData *out_data,PF_ParamDef *params[],PF_LayerDef *output) {
  
	PF_Err err = PF_Err_NONE;
	
	switch (cmd) {
    case PF_Cmd_ABOUT: err = About(in_data,out_data,params,output); break;
    case PF_Cmd_GLOBAL_SETUP: err = GlobalSetup(in_data,out_data,params,output); break;
    case PF_Cmd_PARAMS_SETUP: err = ParamsSetup(in_data,out_data,params,output); break;
    case PF_Cmd_SEQUENCE_SETUP: err = SequenceSetup(in_data,out_data,params,output); break;
    case PF_Cmd_SEQUENCE_SETDOWN: err = SequenceSetdown(in_data,out_data,params,output); break;
    case PF_Cmd_SEQUENCE_RESETUP: err = SequenceResetup(in_data,out_data,params,output); break;
    case PF_Cmd_RENDER: err = Render(in_data,out_data,params,output); break;
	}
	return err;
}

#ifdef AE_OS_WIN
	BOOL WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved) {
		
    HINSTANCE my_instance_handle = (HINSTANCE)0;
		
		switch (dwReason) {
			case DLL_PROCESS_ATTACH: my_instance_handle = hDLL; break;
			case DLL_THREAD_ATTACH:  my_instance_handle = hDLL; break;
			case DLL_THREAD_DETACH:  my_instance_handle = 0; break;
			case DLL_PROCESS_DETACH: my_instance_handle = 0; break;
		}
		return(TRUE);
	}
#endif

