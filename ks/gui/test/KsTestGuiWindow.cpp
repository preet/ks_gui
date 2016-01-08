/*
   Copyright (C) 2015 Preet Desai (preet.desai@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <ks/gl/KsGLCommands.hpp>
#include <ks/gl/KsGLImplementation.hpp>
#include <ks/gui/KsGuiWindow.hpp>
#include <ks/gui/KsGuiApplication.hpp>
#include <ks/platform/KsPlatformMain.hpp>

using namespace ks;

namespace {

    std::atomic<bool> init_gl_state(false);
    unique_ptr<gl::StateSet> gl_state_set;

    Signal<> signal_render_win0;
    Signal<> signal_render_win1;

    void ClearScreenBlue()
    {
        if(!init_gl_state)
        {
            ks::gl::Implementation::GLCapture();
            gl_state_set = make_unique<gl::StateSet>();
            gl_state_set->CaptureState();
            init_gl_state = true;
        }

        gl_state_set->SetClearColor(0,0,1,1);
        gl::Clear(gl::ColorBufferBit);
    }

    void ClearScreenRed()
    {
        if(!init_gl_state)
        {
            ks::gl::Implementation::GLCapture();
            gl_state_set = make_unique<gl::StateSet>();
            gl_state_set->CaptureState();
            init_gl_state = true;
        }

        gl_state_set->SetClearColor(1,0,0,1);
        gl::Clear(gl::ColorBufferBit);
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;


    // Create application
    shared_ptr<gui::Application> app =
            make_object<gui::Application>();

    gui::Window::Attributes win_attribs;
    gui::Window::Properties win_props;


    // Create window using the application's EventLoop (Blue)
    shared_ptr<gui::Window> win0 =
            app->CreateWindow(
                app->GetEventLoop(),
                win_attribs,
                win_props);

    shared_ptr<gui::Layer> win0_layer =
            win0->CreateLayer(0);

    win0_layer->signal_render.Connect(
                &ClearScreenBlue,
                nullptr,
                ConnectionType::Direct);

    signal_render_win0.Connect(
                win0,
                &gui::Window::Render,
                ConnectionType::Queued);

    shared_ptr<CallbackTimer> win0_timer =
            make_object<CallbackTimer>(
                app->GetEventLoop(),
                Milliseconds(16),
                [win0_layer](){
                    signal_render_win0.Emit();
                });

    win0_timer->Start();


    // Create window using a dedicated render thread (Red)
    shared_ptr<EventLoop> win1_render_evl =
            make_shared<EventLoop>();

    std::thread win1_render_thread =
            EventLoop::LaunchInThread(win1_render_evl);

    shared_ptr<gui::Window> win1 =
            app->CreateWindow(
                win1_render_evl,
                win_attribs,
                win_props);

    shared_ptr<gui::Layer> win1_layer =
            win1->CreateLayer(0);

    win1_layer->signal_render.Connect(
                &ClearScreenRed,
                nullptr,
                ConnectionType::Direct);

    signal_render_win1.Connect(
                win1,
                &gui::Window::Render,
                ConnectionType::Direct);

    shared_ptr<CallbackTimer> win1_timer =
            make_object<CallbackTimer>(
                win1_render_evl,
                Milliseconds(16),
                [win1_layer](){
                    signal_render_win1.Emit();
                });

    win1_timer->Start();


    // Run!
    app->Run();

    EventLoop::RemoveFromThread(
                win1_render_evl,win1_render_thread,true);

    return 0;
}
