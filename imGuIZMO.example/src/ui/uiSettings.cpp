///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//  
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////
#include "../glApp.h"

#include "../libs/ImGui/imgui.h"


//Dark + Blu
void colorTheme5b()
{
    ImGuiStyle& style = ImGui::GetStyle();
    //Like Solar
    //ImVec4 wBG(0.00f, 0.045f, 0.0525f, 1.00f);
    //ImVec4 wBG(0.00f, 0.06f, 0.075f, 1.00f);
    //ImVec4 wBG(0.00f, 0.12f, 0.15f, 1.00f);
    ImVec4 wBG(0.00f, 0.09f, 0.12f, 1.00f);
    
    //Blu
    //ImVec4 wBG(0.00f, 0.03f, 0.09f, 1.00f);
    //ImVec4 wBG(0.00f, 0.045f, 0.075f, 1.00f);
    //ImVec4 wBG(0.00f, 0.06f, 0.09f, 1.00f);
    //ImVec4 wBG(0.07f, 0.07f, 0.00f, 1.00f);
    
    //0.00,0.09,0.16

    //ImVec4 clA(0.17f, 0.32f, 0.35f, 1.00f); //0.00f, 0.55f, 0.87f
    
    ImVec4 clA(wBG.x*3.5f, wBG.y*3.5f, wBG.z*3.5f, 1.00f); //0.00f, 0.55f, 0.87f
    ImVec4 clB(wBG.x*2.f, wBG.y*2.f, wBG.z*2.f, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clC(wBG.x*5.f, wBG.y*5.f, wBG.z*5.f, 1.00f); //0.20f, 0.22f, 0.27f
    
    //ImVec4 txt(0.55f, 0.70f, 0.70f, 1.00f); //0.86f, 0.93f, 0.89f       
    ImVec4 act(0.00f, 0.33f, 0.66f, 1.00f); //0.86f, 0.93f, 0.89f
    //ImVec4 act(wBG.x*5.f, wBG.y*5.f, wBG.z*5.f, 1.00f);

    ImVec4 txt;
    ImVec4 HSV;
    ImGui::ColorConvertRGBtoHSV(wBG.x, wBG.y, wBG.z, HSV.x, HSV.y, HSV.z);
    if(HSV.y>.25) HSV.y=.25; 
    HSV.z=.75;
    ImGui::ColorConvertHSVtoRGB(HSV.x, HSV.y, HSV.z, txt.x, txt.y, txt.z);

    


    style.Colors[ImGuiCol_Text]                  = ImVec4(txt.x, txt.y, txt.z, 1.00f);
	style.Colors[ImGuiCol_TextDisabled]          = ImVec4(txt.x, txt.y, txt.z, 0.45f);
	style.Colors[ImGuiCol_WindowBg]              = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
	style.Colors[ImGuiCol_ChildBg]         = ImVec4(clB.x, clB.y, clB.z, 0.30f);
	style.Colors[ImGuiCol_Border]                = ImVec4(clB.x, clB.y, clB.z, 0.75f);
	style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]               = ImVec4(clC.x, clC.y, clC.z, 0.25f);
	style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(clC.x, clC.y, clC.z, 0.70f);
	style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
	style.Colors[ImGuiCol_TitleBg]               = ImVec4(clB.x, clB.y, clB.z, 0.70f);
	style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(wBG.x, wBG.y, wBG.z, 0.50f);
	style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(act.x, act.y, act.z, 0.90f);
	style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(clB.x, clB.y, clB.z, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(clB.x, clB.y, clB.z, 0.25f);
	style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(clA.x, clA.y, clA.z, 0.50f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(clA.x, clA.y, clA.z, 0.75f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(clA.x, clA.y, clA.z, 1.00f);
	//style.Colors[ImGuiCol_ComboBg]               = ImVec4(clB.x, clB.y, clB.z, 1.00f);
	style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab]            = ImVec4(clC.x, clC.y, clC.z, 0.55f);
	style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(clC.x, clC.y, clC.z, 0.70f);
	style.Colors[ImGuiCol_Button]                = ImVec4(clC.x, clC.y, clC.z, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(clC.x, clC.y, clC.z, 0.50f);
	style.Colors[ImGuiCol_ButtonActive]          = ImVec4(act.x, act.y, act.z, 1.00f);
	style.Colors[ImGuiCol_Header]                = ImVec4(act.x, act.y, act.z, 0.50f);
	style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(act.x, act.y, act.z, 0.75f);
	style.Colors[ImGuiCol_HeaderActive]          = ImVec4(act.x, act.y, act.z, 1.00f);
	style.Colors[ImGuiCol_Separator]                = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
	style.Colors[ImGuiCol_SeparatorHovered]         = ImVec4(clA.x, clA.y, clA.z, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive]          = ImVec4(clA.x, clA.y, clA.z, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(clB.x, clB.y, clB.z, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(clA.x, clA.y, clA.z, 0.78f);
	style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(clA.x, clA.y, clA.z, 1.00f);
	//style.Colors[ImGuiCol_CloseButton]           = ImVec4(txt.x, txt.y, txt.z, 0.16f);
	//style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(txt.x, txt.y, txt.z, 0.39f);
	//style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(txt.x, txt.y, txt.z, 1.00f);
	style.Colors[ImGuiCol_PlotLines]             = ImVec4(txt.x, txt.y, txt.z, 0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(clA.x, clA.y, clA.z, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(txt.x, txt.y, txt.z, 0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(clA.x, clA.y, clA.z, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(clA.x, clA.y, clA.z, 0.43f);
	style.Colors[ImGuiCol_PopupBg]				 = ImVec4(clB.x, clB.y, clB.z, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDarkening]  = ImVec4(clB.x, clB.y, clB.z, 0.73f);
}




void setGUIStyle(void)
{
    ImGuiStyle& style = ImGui::GetStyle();

    //SetupImGuiStyle2();
    colorTheme5b();
    style.ItemSpacing = ImVec2(2,3); 
    style.ItemInnerSpacing = ImVec2(2,3); 

    style.FrameRounding = 3;
    style.WindowRounding = 5;
    style.ScrollbarSize = 15;
    style.GrabRounding = 3;
    style.WindowPadding = ImVec2(3,3);
    style.ChildRounding = 3;
    style.ScrollbarRounding = 5;
    style.FramePadding = ImVec2(3,3);

    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
    //style.FrameBorderSize = .0f;
    
    //style.WindowTitleAlign = ImVec2(.03f,.5f);
    


    ImFontConfig cfg;
    //memset(&cfg, 0, sizeof(ImFontConfig));
    cfg.OversampleH = 3.;
    cfg.OversampleV = 1.;
    cfg.PixelSnapH = true;
    cfg.GlyphExtraSpacing.x = 0.0f;
    cfg.GlyphExtraSpacing.y = 0.0f;
    cfg.RasterizerMultiply = 1.25f;

    //ImVec4 color(style.Colors[ImGuiCol_FrameBg]);

    //ImVec4 v(style.Colors[ImGuiCol_FrameBg]);
//    theDlg.setTableAlterbateColor1(ImVec4(v.x*1.1,v.y*1.1,v.z*1.1,v.w));
    //theDlg.setTableAlterbateColor2(ImVec4(v.x*.9,v.y*.9,v.z*.9,v.w));


}

