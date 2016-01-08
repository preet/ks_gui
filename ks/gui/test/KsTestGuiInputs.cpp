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

    Signal<> signal_render_win;

    void ClearScreen()
    {
        if(!init_gl_state)
        {
            ks::gl::Implementation::GLCapture();
            gl_state_set = make_unique<gl::StateSet>();
            gl_state_set->CaptureState();
            init_gl_state = true;
        }

        gl_state_set->SetClearColor(0,0,0.5,1);
        gl::Clear(gl::ColorBufferBit);
    }

    void PrintKeyPress(gui::KeyEvent event)
    {
        LOG.Trace() << "KeyEvent: "
                    << "key: " << static_cast<uint>(event.key) << ", "
                    << "scancode: " << event.scancode << ", "
                    << "action: " << static_cast<uint>(event.action) << ", "
                    << "mods: " << static_cast<uint>(event.mods);
    }

    void PrintTextInput(std::string s)
    {
        LOG.Trace() << "TextInput: " << s;
    }

    void PrintMouseOutput(gui::MouseEvent event)
    {
        LOG.Trace() << "MouseEvent: "
                    << "button: " << static_cast<uint>(event.button) << ", "
                    << "action: " << static_cast<uint>(event.action) << ", "
                    << "x: " << event.x << ", y: " << event.y;
    }

    void PrintScrollOutput(gui::ScrollEvent event)
    {
        LOG.Trace() << "ScrollEvent: x: " << event.x << ", y: " << event.y;
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


    // Create window using the application's EventLoop
    shared_ptr<gui::Window> win =
            app->CreateWindow(
                app->GetEventLoop(),
                win_attribs,
                win_props);

    shared_ptr<gui::Layer> win_layer =
            win->CreateLayer(0);

    win_layer->signal_render.Connect(
                &ClearScreen,
                nullptr,
                ConnectionType::Direct);

    signal_render_win.Connect(
                win,
                &gui::Window::Render,
                ConnectionType::Queued);

    shared_ptr<CallbackTimer> win_timer =
            make_object<CallbackTimer>(
                app->GetEventLoop(),
                Milliseconds(16),
                [win_layer](){
                    signal_render_win.Emit();
                });

    win_timer->Start();


    // Connect Input signals
    app->signal_keyboard_input->Connect(
                &PrintKeyPress);

    app->signal_utf8_input->Connect(
                &PrintTextInput);

    app->signal_mouse_input->Connect(
                &PrintMouseOutput);

    app->signal_scroll_input->Connect(
                &PrintScrollOutput);


    // Run!
    app->Run();


    return 0;
}
