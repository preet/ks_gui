/*
   Copyright (C) 2015-2016 Preet Desai (preet.desai@gmail.com)

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

// Testing out opening and drawing to window

// NOTE: Multiple windows are experimental and
// haven't been tested that well yet

using namespace ks;

namespace
{
    class Scene : public ks::Object
    {
    public:
        using base_type = ks::Object;

        Scene(ks::Object::Key const &key,
              shared_ptr<EventLoop> evl,
              gui::Window* win0,
              gui::Window* win1) :
            ks::Object(key,evl),
            m_win0(win0),
            m_win1(win1)
        {
            auto gl_clear = [](){ gl::Clear(gl::ColorBufferBit); };

            m_render_task_fn0 =
                    [win0,gl_clear](){
                        win0->InvokeWithContext(gl_clear);
                        win0->SwapBuffers();
                    };

            m_render_task_fn1 =
                    [win1,gl_clear](){
                        win1->InvokeWithContext(gl_clear);
                        win1->SwapBuffers();
                    };
        }

        void Init(ks::Object::Key const &,
                  shared_ptr<Scene> const &)
        {}

        ~Scene()
        {}

        void OnAppInit()
        {
            m_running = true;
            signal_app_process_events.Emit();

            // Capture GL implementation info
            auto gl_capture = [](){ ks::gl::Implementation::GLCapture(); };

            auto init_gl_task =
                    make_shared<ks::Task>(
                        [this,gl_capture](){
                            m_win0->InvokeWithContext(gl_capture);
                        });

            m_win0->GetEventLoop()->PostTask(init_gl_task);
            init_gl_task->Wait();


            // Set the GL Clear color state for each window
            m_gl_state_set0 = make_unique<gl::StateSet>();
            m_gl_state_set1 = make_unique<gl::StateSet>();

            auto set_red =
                    [this](){
                        m_gl_state_set0->CaptureState();
                        m_gl_state_set0->SetClearColor(1,0,0,1);
                    };

            auto set_red_task =
                    make_shared<ks::Task>(
                        [this,set_red](){
                            m_win0->InvokeWithContext(set_red);
                        });

            m_win0->GetEventLoop()->PostTask(set_red_task);
            set_red_task->Wait();

            auto set_blue =
                    [this](){
                        m_gl_state_set1->CaptureState();
                        m_gl_state_set1->SetClearColor(0,0,1,1);
                    };

            auto set_blue_task =
                    make_shared<ks::Task>(
                        [this,set_blue](){
                            m_win1->InvokeWithContext(set_blue);
                        });

            m_win1->GetEventLoop()->PostTask(set_blue_task);
            set_blue_task->Wait();
        }

        void OnAppPause()
        {
            m_running = false;
        }

        void OnAppResume()
        {
            m_running = true;
            signal_app_process_events.Emit();
        }

        void OnAppQuit()
        {
            m_running = false;
        }

        void OnAppProcEvents(bool)
        {
            if(m_running)
            {
                auto render_task0 = make_shared<ks::Task>(m_render_task_fn0);
                m_win0->GetEventLoop()->PostTask(render_task0);

                auto render_task1 = make_shared<ks::Task>(m_render_task_fn1);
                m_win1->GetEventLoop()->PostTask(render_task1);

                render_task0->Wait();
                render_task1->Wait();

                // We're assuming vsync is true and will introduce
                // a delay so we schedule the next ProcessEvents task
                // immediately
                signal_app_process_events.Emit();
            }
        }

        Signal<> signal_app_process_events;


    private:
        std::atomic<bool> m_running;

        gui::Window* m_win0;
        gui::Window* m_win1;

        std::function<void()> m_render_task_fn0;
        std::function<void()> m_render_task_fn1;

        unique_ptr<gl::StateSet> m_gl_state_set0;
        unique_ptr<gl::StateSet> m_gl_state_set1;
    };
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Create application
    auto app = MakeObject<gui::Application>();

    // Create windows
    gui::Window::Attributes win_attribs;
    gui::Window::Properties win_props;
    win_props.width = 640;
    win_props.height = 480;
    win_props.swap_interval = 1;

    win_props.title = "Window 0";
    auto win0_render_evl = make_shared<EventLoop>();
    auto win0_render_thread = EventLoop::LaunchInThread(win0_render_evl);
    auto win0 = app->CreateWindow(win0_render_evl,win_attribs,win_props);

    win_props.x = 100;
    win_props.y = 100;
    win_props.title = "Window 1";
    auto win1_render_evl = make_shared<EventLoop>();
    auto win1_render_thread = EventLoop::LaunchInThread(win1_render_evl);
    auto win1 = app->CreateWindow(win1_render_evl,win_attribs,win_props);

    // Create scene
    auto scene =
            MakeObject<Scene>(
                app->GetEventLoop(),
                win0.get(),
                win1.get());

    // Setup connections

    // Application ---> Scene
    app->signal_init.Connect(
                scene,
                &Scene::OnAppInit);

    app->signal_pause.Connect(
                scene,
                &Scene::OnAppPause,
                ks::ConnectionType::Direct);

    app->signal_resume.Connect(
                scene,
                &Scene::OnAppResume);

    app->signal_quit.Connect(
                scene,
                &Scene::OnAppQuit,
                ks::ConnectionType::Direct);

    app->signal_processed_events->Connect(
                scene,
                &Scene::OnAppProcEvents);


    // Scene ---> Application
    scene->signal_app_process_events.Connect(
                app,
                &gui::Application::ProcessEvents);


    // Run!
    app->Run();

    // Clean up threads
    EventLoop::RemoveFromThread(win0_render_evl,win0_render_thread,true);
    EventLoop::RemoveFromThread(win1_render_evl,win1_render_thread,true);

    return 0;
}
