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


#include <imguizmo_quat.h>

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

bool isVisible = true;


void renderWidgets(vg::vGizmo3D &track, vec3& vLight, int width, int height)
{

    static bool metricW = false, demosW = false;
    if(!isVisible) return;

   ImGuiStyle& style = ImGui::GetStyle();
           
   // temporary vec3

   ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
   ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_FirstUseEver);

    bool isVisible = true;


// Right Side Widgets
/////////////////////////////////////////////////////////////////////
    float widgetSize=240;
    ImGui::SetNextWindowSize(ImVec2(widgetSize, height), ImGuiCond_Always); // top ...
    ImGui::SetNextWindowPos(ImVec2(width-widgetSize, 0), ImGuiCond_Always); // ... right aligned

    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.f,0.f,0.f,0.f));       // transparent Wnd Background
    ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(0.f,0.f,0.f,0.f));        // transparent frame Background
    const float prevWindowBorderSize = style.WindowBorderSize;              // Save current border size...
    style.WindowBorderSize = .0f;                                           // ... to draw the window with ZERO broder

    ImGui::Begin("##giz", &isVisible, ImGuiWindowFlags_NoTitleBar|          // noTitle / noResize / Noscrollbar
                                      ImGuiWindowFlags_NoResize|
                                      ImGuiWindowFlags_NoScrollbar);

    

    
    ImGui::SetCursorPos(ImVec2(0,0));
    ImGui::PushItemWidth(widgetSize*.25-2);
    ImGui::TextColored(ImVec4(1,1,1,1), "w: % 1.2f", track.getRotation().w); ImGui::SameLine();
    ImGui::TextColored(ImVec4(1,0,0,1), "x: % 1.2f", track.getRotation().x); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,1,0,1), "y: % 1.2f", track.getRotation().y); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0,0,1,1), "z: % 1.2f", track.getRotation().z);
    ImGui::PopItemWidth();


    // Upper right corner widget
    ///////////////////////////////////
    ImGui::gizmo3D("##aaa", track.refRotation(), vLight, widgetSize);

    widgetSize*=.5;

    // Other right side widgets
    ///////////////////////////////////

    // Pan & Dolly axes
    ImGui::gizmo3D("Pan & Dolly", track.refPosition(), track.refRotation(), widgetSize);

    ImGui::SameLine();

    // Directional light
    if(ImGui::gizmo3D("##Dir1", vLight, widgetSize, imguiGizmo::sphereAtOrigin) );

    static bool otherExamples = false;

    ImVec2 cPos(ImGui::GetCursorPos());

    ImGui::SetCursorPos(ImVec2(0,height-ImGui::GetFrameHeightWithSpacing()*15));

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

    ImGui::SetCursorPos(ImVec2(0,height-ImGui::GetFrameHeightWithSpacing()));
    if(ImGui::Button(" -= Show more examples =- ")) otherExamples ^=1;

    // static vars used to modify widget aspect
    static float resSolid = 1.0;
    static float axesLen = .95;
    static float axesThickness = 1.0;
    vec3 resAxes(axesLen,axesThickness,axesThickness);
    static vec3 dirCol(1.0,1.0,1.0);
    static vec4 planeCol(.75,.0,0.0, STARTING_ALPHA_PLANE);
    static ImVec4 sphCol1(ImGui::ColorConvertU32ToFloat4(0xff0080ff));
    static ImVec4 sphCol2(ImGui::ColorConvertU32ToFloat4(0xffff8000));


    // other widgets to show (not all connected to main scene)
    if(otherExamples) {
        // dummies vars to utilize in dummies widgets
        static quat qt2(1.0f,0,0,0);
        static quat qt3(1.0f,0,0,0);
        static quat qt4(1.0f,0,0,0);

        static vec3 a(1.f);
        static vec4 b(1.0,0.0,0.0,0.0);
        static vec4 c(1.0,0.0,0.0,0.0);
        static vec4 d(1.0,0.0,0.0,0.0);

        char s[50];

        ImGui::SetCursorPos(cPos);
        imguiGizmo::resizeAxesOf(resAxes);
        imguiGizmo::resizeSolidOf(resSolid); // sphere bigger
        if(ImGui::gizmo3D("##RotB", b, widgetSize, imguiGizmo::sphereAtOrigin))  {
            track.setRotation(angleAxis(b.w, vec3(b)));
        }   //
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreAxesSize();

        ImGui::SameLine();

        imguiGizmo::resizeSolidOf(.75); // sphere bigger
        if(ImGui::gizmo3D("##RotB1", qt3, d,widgetSize, imguiGizmo::sphereAtOrigin))  {   }
        imguiGizmo::restoreSolidSize(); // restore at default


        imguiGizmo::resizeAxesOf(vec3(imguiGizmo::axesResizeFactor.x, 1.75, 1.75));
        imguiGizmo::resizeSolidOf(1.5); // sphere bigger
        imguiGizmo::setSphereColors(ImGui::ColorConvertFloat4ToU32(sphCol1), ImGui::ColorConvertFloat4ToU32(sphCol2));
        //c = vec4(axis(qt), angle(qt)); 
        if(ImGui::gizmo3D("##RotC", c,widgetSize, imguiGizmo::sphereAtOrigin|imguiGizmo::modeFullAxes)) {}   //theWnd->getTrackball().setRotation(angleAxis(c.w, vec3(c)));
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreSphereColors();
        imguiGizmo::restoreAxesSize();

        ImGui::SameLine();


        //imguiGizmo::resizeAxesOf(vec3(2.5, 2.5, 2.5));
        imguiGizmo::resizeAxesOf(resAxes);
        imguiGizmo::resizeSolidOf(resSolid); // sphere bigger
        if(ImGui::gizmo3D("##tZ", qt2, qt4, widgetSize, imguiGizmo::modeFullAxes));
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreAxesSize();
        //imguiGizmo::restoreAxesSize();
    
        imguiGizmo::resizeAxesOf(resAxes);
        imguiGizmo::resizeSolidOf(resSolid*2); // sphere bigger
        if(ImGui::gizmo3D("##tZ2", qt2, widgetSize, imguiGizmo::cubeAtOrigin|imguiGizmo::modeFullAxes));
        imguiGizmo::restoreSolidSize(); // restore at default
        imguiGizmo::restoreAxesSize();

        ImGui::SameLine();   

        imguiGizmo::resizeAxesOf(resAxes);
        //this is only direction!!!... and i can change color

        imguiGizmo::setDirectionColor(ImGui::ColorConvertU32ToFloat4(0xff0080ff), ImGui::ColorConvertU32ToFloat4(0xc0ff8000));
        if( ImGui::gizmo3D("##RotA", a,widgetSize, imguiGizmo::modeDirPlane)) {}
        imguiGizmo::restoreDirectionColor();
        imguiGizmo::restoreAxesSize();

    }

    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();

    style.WindowBorderSize = prevWindowBorderSize;

    const int dimY =300;
    ImGui::SetNextWindowSize(ImVec2(540, dimY), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0,height-dimY), ImGuiCond_FirstUseEver);

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
            if(ImGui::SliderFloat(" Mouse", &mouseFeeling, .25, 7.0, "sensitivity %.2f")) imguiGizmo::setGizmoFeelingRot(mouseFeeling);
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

            if(ImGui::Button(" -= Change solids attributes =- ")) vertexChange ^=1;

            ImGui::NextColumn();
            imguiGizmo::resizeAxesOf(resAxes);
            imguiGizmo::resizeSolidOf(resSolid); // sphere bigger
            imguiGizmo::setSphereColors(ImGui::ColorConvertFloat4ToU32(sphCol1), ImGui::ColorConvertFloat4ToU32(sphCol2));            
            imguiGizmo::setDirectionColor(ImVec4(dirCol.x,dirCol.y, dirCol.z, 1.0),ImVec4(planeCol.x,planeCol.y, planeCol.z, planeCol.w));
            //plane & dir with same color - > imguiGizmo::setDirectionColor(ImVec4(dirCol.x,dirCol.y, dirCol.z, 1.0));
            
            static quat qv2(1.0f,0,0,0);

            if(isPanDolly) {
                if(mode & imguiGizmo::modeDual) {
                    ImGui::gizmo3D("pan & zoom mode", track.refRotation(), vLight, w, mode|draw);
                } else {
                    ImGui::gizmo3D("pan & zoom mode", track.refPosition(), track.refRotation(), w, mode|draw );
                }
            } else {
                if(mode & imguiGizmo::modeDual) {
                    ImGui::gizmo3D("##gizmoV2", track.refRotation(), vLight, w, mode|draw);
                } else {
                    ImGui::gizmo3D("##gizmoV1", track.refRotation(), w, mode|draw );
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

                if(needRebuild)  needRebuild = imguiGizmo::solidAreBuilt = false;


            } ImGui::EndGroup();


        } ImGui::End();
    }
    //  statistics
    ///////////////////////////////////////////
                
        if(metricW) ImGui::ShowMetricsWindow(&metricW);
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        if(demosW) ImGui::ShowDemoWindow(&demosW);


}

