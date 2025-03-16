//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include <sstream>

#include "../tools/oglAxes.h"
#include "../glApp.h"
#include "../glWindow.h"


#include "uiMainDlg.h"
#include "../tools/imGuIZMOquat.h"

void setRotation(const quat &q);
void setPosition(const vec3 &p);
#ifdef GLAPP_USE_VIRTUALGIZMO // Enable it in glWindows.h or with compiler define
    quat getRotation();
    vec3 getPosition();
#else
    quat const &getRotation();
    vec3 const &getPosition();
#endif

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void mainImGuiDlgClass::renderImGui()
{

    static bool metricW = false, demosW = false;
    if(!visible()) return;

    ImGui_ImplOpenGL3_NewFrame();
#ifdef GLAPP_USE_SDL
    ImGui_ImplSDL2_NewFrame(theApp->getSDLWWnd());
#else
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();

    ImGuiStyle& style = ImGui::GetStyle();
           
   // temporary vec3
   vec3 tLight = theWnd->getAxes()->getLightPos();

   ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_FirstUseEver);


    bool isVisible = true;
/*
    if(ImGui::Begin("uuu", &isVisible)) {
        static quat qt2(1.0f,0,0,0);
        static quat qt4(1.0f,0,0,0);
        ImGui::gizmo3D("##tZ", qt2, qt4, 800);
    } ImGui::End();
*/        

// Right Side Widgets
/////////////////////////////////////////////////////////////////////
    static bool vis = true;
    quat qt = theWnd->getAxes()->getTransforms()->getTrackball().getRotation();
    vec3 pos = theWnd->getAxes()->getTransforms()->getTrackball().getPosition();

    float sz=240;
    ImGui::SetNextWindowSize(ImVec2(sz, theApp->GetHeight()), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-sz,0 ), ImGuiCond_Always);
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.f,0.f,0.f,0.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(0.f,0.f,0.f,0.f));
    
    const float prevWindowBorderSize = style.WindowBorderSize;
    style.WindowBorderSize = .0f;

    ImGui::Begin("##giz", &vis, ImGuiWindowFlags_NoTitleBar|
                                ImGuiWindowFlags_NoResize|
                                //ImGuiWindowFlags_NoInputs|
                                ImGuiWindowFlags_NoScrollbar);

    

    
    ImGui::SetCursorPos(ImVec2(0,0));
    ImGui::PushItemWidth(sz*.25-2);
    ImVec4 oldTex(style.Colors[ImGuiCol_Text]);
    bool quatChanged=false;
    style.Colors[ImGuiCol_Text].x = 1.0, style.Colors[ImGuiCol_Text].y = style.Colors[ImGuiCol_Text].z =0.f;
    if(ImGui::DragFloat("##u0",(float *)&qt.x,0.01f, -1.0, 1.0, "x: %.2f",1.f)) quatChanged=true; ImGui::SameLine();
    style.Colors[ImGuiCol_Text].y = 1.0, style.Colors[ImGuiCol_Text].x = style.Colors[ImGuiCol_Text].z =0.f;
    if(ImGui::DragFloat("##u1",(float *)&qt.y,0.01f, -1.0, 1.0, "y: %.2f",1.f)) quatChanged=true;  ImGui::SameLine();
    style.Colors[ImGuiCol_Text].z = 1.0, style.Colors[ImGuiCol_Text].y = style.Colors[ImGuiCol_Text].x =0.f;
    if(ImGui::DragFloat("##u2",(float *)&qt.z,0.01f, -1.0, 1.0, "z: %.2f",1.f)) quatChanged=true;  ImGui::SameLine();
    style.Colors[ImGuiCol_Text] = oldTex;
    if(ImGui::DragFloat("##u3",(float *)&qt.w,0.01f, -1.0, 1.0, "w: %.2f",1.f)) quatChanged=true;
    ImGui::PopItemWidth();

    //If you modify quaternion parameters outside control, with DragFloat or other, remember to NORMALIZE it
    if(quatChanged) setRotation(normalize(qt));
    ImGui::DragFloat3("Light",value_ptr(tLight),0.01f);


    // Upper right corner widget
    ///////////////////////////////////
    vec3 lL(-tLight);
    if(ImGui::gizmo3D("##aaa", qt, lL, sz))  { 
        tLight = -lL;
        setRotation(qt);
    }

    sz*=.5;
    static quat qt2(1.0f,0,0,0);
    static quat qt3(1.0f,0,0,0);
    static quat qt4(1.0f,0,0,0);

    static vec3 a(1.f);
    static vec4 b(1.0,0.0,0.0,0.0);
    static vec4 c(1.0,0.0,0.0,0.0);
    static vec4 d(1.0,0.0,0.0,0.0);

    static float resSolid = 1.0;
    static float axesLen = .95;
    static float axesThickness = 1.0;
    vec3 resAxes(axesLen,axesThickness,axesThickness);
    static vec3 dirCol(1.0,1.0,1.0);
    static vec4 planeCol(.75,.0,0.0, STARTING_ALPHA_PLANE);
    static ImVec4 sphCol1(ImGui::ColorConvertU32ToFloat4(0xff0080ff));
    static ImVec4 sphCol2(ImGui::ColorConvertU32ToFloat4(0xffff8000));

    //ImGui::PushItemWidth(sz);
    //quat form gizmo is normalized!
    char s[50];

    // Other rigth widgets
    ///////////////////////////////////
    {
#ifndef IMGUIZMO_USE_ONLY_ROT // Widget with Pan & Dolly
    ///////////////////////////////////////////////////////
    // example of direct call ==> works w/o GLAPP_USE_VIRTUALGIZMO
    //////////////////////////////////
    //extern vec3 position; 
    //extern quat qRot;
    //ImGui::gizmo3D("##gizmo1", position, qRot, sz);

    ///////////////////////////////////////////////////////
    // using helper funcs 
    //vec3 pos = getPosition(); // my saved position
    //quat qt = getRotation();
    if(ImGui::gizmo3D("Pan & Dolly", pos, qt, sz)) { setRotation(qt); setPosition(pos); }
#else
    quat qt = getRotation();
    if(ImGui::gizmo3D("##gizmo1", qt, sz)) { setRotation(qt); }
#endif
    }

    ImGui::SameLine();

    vec3 vL(-tLight);
    if(ImGui::gizmo3D("##Dir1", vL,sz, imguiGizmo::sphereAtOrigin) )  tLight = -vL;

    static bool otherExamples = false;

    ImVec2 cPos(ImGui::GetCursorPos());

    ImGui::SetCursorPos(ImVec2(0,theApp->GetHeight()-ImGui::GetFrameHeightWithSpacing()*15));

    if(!otherExamples) {
        ImGui::TextColored(ImVec4(1.f, .75f, 0.f, 1.f),"    imGuIZMO.quad MOUSE usage");
        ImGui::NewLine();
        ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f),"Main rotation:");
        ImGui::Text("- Left  btn -> free rotation");
        ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f),"Only for AXES with SPOT widget:");
        ImGui::Text("- Right btn -> free rotation spot");
        ImGui::Text("- Middle/Both btns -> rot both");
        ImGui::NewLine();
        ImGui::TextColored(ImVec4(0.f, 1.f, 1.f, 1.f),"Based on WIDGET TYPE it can also:");
        ImGui::NewLine();
        ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f),"or Rotation around a fixed axis:");
        ImGui::Text("- Shft+LBtn -> rot ONLY around X");
        ImGui::Text("- Ctrl+LBtn -> rot ONLY around Y");
        ImGui::Text("- Alt|Super+LBtn -> rot around Z");
        ImGui::NewLine();
        ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f),"or Pan & Dolly (when active):");
        ImGui::Text("- Shft+LBtn -> Dolly/Zoom");
        ImGui::Text("- Wheel     -> Dolly/Zoom");
        ImGui::Text("- Ctrl+LBtn -> Pan/Move");
    }

    ImGui::SetCursorPos(ImVec2(0,theApp->GetHeight()-ImGui::GetFrameHeightWithSpacing()));
    if(ImGui::Button(" -= Show more examples =- ")) otherExamples ^=1;

    if(otherExamples) {

        ImGui::SetCursorPos(cPos);
        imguiGizmo::resizeAxesOf(resAxes);
        imguiGizmo::resizeSolidOf(resSolid); // sphere bigger
        if(ImGui::gizmo3D("##RotB", b,sz, imguiGizmo::sphereAtOrigin))  {
            setRotation(angleAxis(b.w, vec3(b)));   
        }   //
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreAxesSize();

        ImGui::SameLine();

        imguiGizmo::resizeSolidOf(.75); // sphere bigger
        if(ImGui::gizmo3D("##RotB1", qt3, d,sz, imguiGizmo::sphereAtOrigin))  {   } 
        imguiGizmo::restoreSolidSize(); // restore at default


        imguiGizmo::resizeAxesOf(vec3(imguiGizmo::axesResizeFactor.x, 1.75, 1.75));
        imguiGizmo::resizeSolidOf(1.5); // sphere bigger
        imguiGizmo::setSphereColors(ImGui::ColorConvertFloat4ToU32(sphCol1), ImGui::ColorConvertFloat4ToU32(sphCol2));
        //c = vec4(axis(qt), angle(qt)); 
        if(ImGui::gizmo3D("##RotC", c,sz, imguiGizmo::sphereAtOrigin|imguiGizmo::modeFullAxes)) {}   //theWnd->getTrackball().setRotation(angleAxis(c.w, vec3(c)));   
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreSphereColors();
        imguiGizmo::restoreAxesSize();

        ImGui::SameLine();


        //imguiGizmo::resizeAxesOf(vec3(2.5, 2.5, 2.5));
        imguiGizmo::resizeAxesOf(resAxes);
        imguiGizmo::resizeSolidOf(resSolid); // sphere bigger
        if(ImGui::gizmo3D("##tZ", qt2, qt4, sz, imguiGizmo::modeFullAxes)) { setRotation(qt); }
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreAxesSize();
        //imguiGizmo::restoreAxesSize();
    
        imguiGizmo::resizeAxesOf(resAxes);
        imguiGizmo::resizeSolidOf(resSolid*2); // sphere bigger
        if(ImGui::gizmo3D("##tZ2", qt2, sz, imguiGizmo::cubeAtOrigin|imguiGizmo::modeFullAxes)) { setRotation(qt); }
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreAxesSize();

        ImGui::SameLine();   

        imguiGizmo::resizeAxesOf(resAxes);
        //this is only direction!!!... and i can change color

        imguiGizmo::setDirectionColor(ImGui::ColorConvertU32ToFloat4(0xff0080ff), ImGui::ColorConvertU32ToFloat4(0xc0ff8000));
        if( ImGui::gizmo3D("##RotA", a,sz, imguiGizmo::modeDirPlane)) {}   
        imguiGizmo::restoreDirectionColor();
        imguiGizmo::restoreAxesSize();

    }


    
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    style.WindowBorderSize = prevWindowBorderSize;

    
    const int dimY =300;
    ImGui::SetNextWindowSize(ImVec2(540, dimY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0,theWnd->GetHeight()-dimY), ImGuiCond_FirstUseEver);

    static bool vertexChange = false;

// imGuIZMO.quat - Window Settings 
/////////////////////////////////////////////////////////////////////
    if(ImGui::Begin("gizmo options", &isVisible, ImGuiWindowFlags_NoScrollbar)) {
        ImGui::BeginGroup(); {
            ImGui::Columns(2);
            const float w = ImGui::GetContentRegionAvail().x;
            const float half = w/2.f;
            const float third = w/3.f;

            static int mode_idx = 0;
            static int mode = imguiGizmo::mode3Axes;

            static int draw_idx = 0;
            static int draw = imguiGizmo::cubeAtOrigin;

            if (ImGui::Combo("Modes##combo", &mode_idx, "Axes (default)\0"\
                                                              "Direction\0"\
                                                              "Plane Direction\0"\
                                                              "Dual mode\0"\
                            )) 
            {
                switch (mode_idx)
                {
                    case 0: mode = imguiGizmo::mode3Axes; break;
                    case 1: mode = imguiGizmo::modeDirection; break;
                    case 2: mode = imguiGizmo::modeDirPlane; break;
                    case 3: mode = imguiGizmo::modeDual;  break;

                }
            }


            if (ImGui::Combo("Apparence##combo", &draw_idx,  "Cube in center (default)\0"\
                                                              "Sphere in center\0"\
                                                              "no solids in center\0"\
                            )) 
            {
                switch (draw_idx)
                {
                    case 0: draw = imguiGizmo::cubeAtOrigin; break;
                    case 1: draw = imguiGizmo::sphereAtOrigin; break;
                    case 2: draw = imguiGizmo::noSolidAtOrigin;  break;
                }
            }
            static bool isFull;
            ImGui::Checkbox("Show full axes (default false)", &isFull); 

            ImGui::Text(" ");
            ImGui::Text(" Axes/Arrow/Solids resize");
            ImGui::PushItemWidth(third);    
            ImGui::DragFloat("##res1",&axesLen ,0.01f, 0.0, 1.0, "len %.2f");
            ImGui::SameLine();
            ImGui::DragFloat("##res2",&axesThickness ,0.01f, 0.0, 8.0, "thick %.2f");
            ImGui::SameLine();
            ImGui::DragFloat("##res3",&resSolid ,0.01f, 0.0, 8.0, "solids %.2f");
            ImGui::PopItemWidth();

            if(!(mode & imguiGizmo::mode3Axes) ) {
                if(mode & imguiGizmo::modeDirection) {
                    ImGui::Text(" Direction color");
                    ImGui::ColorEdit3("##Direction",value_ptr(dirCol));
                } else if(mode & imguiGizmo::modeDirPlane) {
                    ImGui::Text(" Arrow color");
                    ImGui::ColorEdit3("##Direction",value_ptr(dirCol));
                    ImGui::Text(" Plane color");
                    ImGui::ColorEdit4("##Plane",value_ptr(planeCol));
                }
            }
    
            if((draw & imguiGizmo::sphereAtOrigin) && !(mode & imguiGizmo::modeDirection)) {
                ImGui::Text(" Color Sphere");
                ImGui::PushItemWidth(half);    
                ImGui::ColorEdit4("##Sph1",(float *) &sphCol1);
                //ImGui::SameLine();
                ImGui::ColorEdit4("##Sph2",(float *) &sphCol2);
                ImGui::PopItemWidth();
            } 



            if(isFull) draw |= imguiGizmo::modeFullAxes;
            else       draw &= ~imguiGizmo::modeFullAxes;


            ImGui::SetCursorPos(ImVec2(0,dimY-ImGui::GetFrameHeightWithSpacing()*4));

            static float mouseFeeling = imguiGizmo::getGizmoFeelingRot(); // default 1.0
            if(ImGui::SliderFloat(" Mouse", &mouseFeeling, .25, 2.0, "sensitivity %.2f")) imguiGizmo::setGizmoFeelingRot(mouseFeeling);
#ifndef IMGUIZMO_USE_ONLY_ROT
            static bool isPanDolly = false;
            ImVec4 col(1.f, 0.5f, 0.5f, 1.f);
            ImGui::TextColored(col,"Pan & Dolly "); ImGui::SameLine();
            ImGui::Checkbox("##Pan & Dolly", &isPanDolly);
            if(isPanDolly) {
                ImGui::SameLine(); ImGui::Text(" (Ctrl / Shift)");
                float panScale = imguiGizmo::getPanScale(), dollyScale = imguiGizmo::getDollyScale();
                ImGui::PushItemWidth(half);
                if(ImGui::SliderFloat("##PanScale", &panScale, .1, 5.0, "panScale %.2f")) imguiGizmo::setPanScale(panScale);
                ImGui::SameLine();
                if(ImGui::SliderFloat("##DollyScale", &dollyScale, .1, 5.0, "dollyScale %.2f")) imguiGizmo::setDollyScale(dollyScale);
                ImGui::PopItemWidth();
            } else {
                ImGui::AlignTextToFramePadding();
                ImGui::NewLine();
            }
#else
                ImGui::AlignTextToFramePadding();
                ImGui::NewLine();
                ImGui::AlignTextToFramePadding();
                ImGui::NewLine();
                ImGui::AlignTextToFramePadding();
                ImGui::NewLine();
#endif

            if(ImGui::Button(" -= Change solids attributes =- ")) vertexChange ^=1;


            ImGui::NextColumn();
            imguiGizmo::resizeAxesOf(resAxes);
            imguiGizmo::resizeSolidOf(resSolid); // sphere bigger
            imguiGizmo::setSphereColors(ImGui::ColorConvertFloat4ToU32(sphCol1), ImGui::ColorConvertFloat4ToU32(sphCol2));            
            imguiGizmo::setDirectionColor(ImVec4(dirCol.x,dirCol.y, dirCol.z, 1.0),ImVec4(planeCol.x,planeCol.y, planeCol.z, planeCol.w));
            //plane & dir with same color - > imguiGizmo::setDirectionColor(ImVec4(dirCol.x,dirCol.y, dirCol.z, 1.0)); 


            quat &qv1=qt; // my saved rotation
            vec3 vL(-tLight);        // my light 

            static quat qv2(1.0f,0,0,0);

#ifndef IMGUIZMO_USE_ONLY_ROT
            if(isPanDolly) {
                //vec3 pos = getPosition(); // my saved position os space
                if(mode & imguiGizmo::modeDual) {
                    if(ImGui::gizmo3D("pan & zoom mode", qv1, vL, w, mode|draw)) { setPosition(pos); setRotation(qv1); tLight = -vL; }
                } else {
                    if(ImGui::gizmo3D("pan & zoom mode", pos, qv1, w, mode|draw )) { setPosition(pos); setRotation(qv1); }
                }
            } else
#endif
            {
                if(mode & imguiGizmo::modeDual) {
                    if(ImGui::gizmo3D("##gizmoV2", qv1, vL, w, mode|draw)) { setRotation(qv1); tLight = -vL; }
                } else {
                    if(ImGui::gizmo3D("##gizmoV1", qv1, w, mode|draw )) { setRotation(qv1); }
                }
            }

            imguiGizmo::restoreSolidSize();
            imguiGizmo::restoreDirectionColor();
            imguiGizmo::restoreAxesSize();
            imguiGizmo::restoreSphereColors();


            } ImGui::EndGroup();


        } ImGui::End();


    const int dimYz =200;
    ImGui::SetNextWindowSize(ImVec2(450, dimYz), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(250,0), ImGuiCond_FirstUseEver);
    if(vertexChange) {
        if(ImGui::Begin("Vertex rebuild", &vertexChange)) {
            ImGui::BeginGroup(); {
                ImGui::Columns(2);
                ImGui::TextWrapped("All vertexes of all solids (cube, cone, cyl, sphere) are processed only once on startup\
 and are invariant for all widgets in application.\n\
Although the proportion can be modified for each individual control (as already seen), the ratio and # faces are fixed.\n\
Anyhow the static variables can be modified to change the 3d aspect of all solids, and rebuild they... una tantum");

                ImGui::NextColumn();
                static bool needRebuild=false;
                if(ImGui::SliderInt("Cone Slices", &imguiGizmo::coneSlices, 3, 30)) needRebuild=true;
                if(ImGui::SliderFloat("Cone Len", &imguiGizmo::coneLength, 0.01, .5))  needRebuild=true;
                if(ImGui::SliderFloat("Cone Radius", &imguiGizmo::coneRadius, 0.01, .3))  needRebuild=true;

                if(ImGui::SliderInt("Cyl Slices", &imguiGizmo::cylSlices, 3, 30))  needRebuild=true;
                if(ImGui::SliderFloat("Cyl Radius", &imguiGizmo::cylRadius, 0.001, .5))  needRebuild=true;

                static int sphTess_idx = 2;
                if (ImGui::Combo("SphereTessel##combo", &sphTess_idx,  "16x32\0"\
                                                                       "8x16\0"\
                                                                       "4x8 (default)\0"\
                                                                       "2x4\0"\
                                )) 
                {
                    switch (sphTess_idx)
                    {
                        case imguiGizmo::sphereTess16: imguiGizmo::sphereTessFactor = imguiGizmo::sphereTess16; break;
                        case imguiGizmo::sphereTess8 : imguiGizmo::sphereTessFactor = imguiGizmo::sphereTess8 ; break;
                        case imguiGizmo::sphereTess4 : imguiGizmo::sphereTessFactor = imguiGizmo::sphereTess4 ; break;
                        case imguiGizmo::sphereTess2 : imguiGizmo::sphereTessFactor = imguiGizmo::sphereTess2 ; break;
                    }
                    needRebuild=true;
                }

                // sizeCylLength = defined in base to control size minus coneLenght

                if(needRebuild)  needRebuild = imguiGizmo::solidAreBuilded = false;


            } ImGui::EndGroup();


        } ImGui::End();
    }
    //  statistics
    ///////////////////////////////////////////
                
        if(metricW) ImGui::ShowMetricsWindow(&metricW);
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        if(demosW) ImGui::ShowDemoWindow(&demosW);

    //reswitch
    theWnd->getAxes()->getLightPos().x = tLight.x;
    theWnd->getAxes()->getLightPos().y = tLight.y;
    theWnd->getAxes()->getLightPos().z = tLight.z;

    theWnd->getAxes()->getTransforms()->getTrackball().setRotation(qt);
    theWnd->getAxes()->getTransforms()->getTrackball().setPosition(pos);


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

