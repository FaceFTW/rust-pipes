mod config;
mod core;
mod rng;
mod teapot;
mod util;
mod world;

use crate::engine::{config::Configuration, util::InstantShim};
use cfg_if::cfg_if;
use three_d::{
    degrees, vec3, Camera, ClearState, Context, DirectionalLight, FrameOutput, OrbitControl, Srgba,
    Window, WindowSettings,
};

use self::core::Engine;

pub fn real_main() {
    //===============================================
    // WORLD INITIALIZATION
    //===============================================
    let cfg: Configuration = get_config();

    //===============================================
    // ENGINE INIT
    //===============================================
    let window = Window::new(WindowSettings {
        title: "Rust Pipes".to_string(),
        max_size: cfg.window_size,
        ..Default::default()
    })
    .unwrap();
    let context: Context = window.gl();

    let cam_y = cfg.world.max_bounds.1 / 2;
    let cam_xz: i8 = -1 * cfg.world.max_bounds.0 as i8;
    let mut camera = Camera::new_perspective(
        window.viewport(),
        vec3(cam_xz as f32, cam_y as f32, cam_xz as f32),
        vec3(0.0, cam_y as f32, 0.0),
        vec3(0.0, 1.0, 0.0),
        degrees(45.0),
        0.1,
        10000.0,
    );
    let mut control = OrbitControl::new(*camera.target(), 1.0, 10000.0);

    let light0 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, -0.5, -0.5));
    let light1 = DirectionalLight::new(&context, 1.0, Srgba::WHITE, &vec3(0.0, 0.5, 0.5));

    let mut start_time = InstantShim::now();
    let mut do_reset = false;

    let mut engine = Engine::new(&cfg, &context);
    window.render_loop(move |mut frame_input| {
        camera.set_viewport(frame_input.viewport);
        control.handle_events(&mut camera, &mut frame_input.events);

        if do_reset && !cfg.single_run {
            start_time = InstantShim::now();
            engine.reset_state();
        }

        //World Update Step
        do_reset = engine.update_state(start_time);

        frame_input
            .screen()
            .clear(ClearState::color_and_depth(
                cfg.draw.bg_color.0 as f32 / 255.0,
                cfg.draw.bg_color.1 as f32 / 255.0,
                cfg.draw.bg_color.2 as f32 / 255.0,
                1.0,
                1.0,
            ))
            .render(&camera, engine.get_render_objects(), &[&light0, &light1]);

        FrameOutput::default()
    });
}

cfg_if! {
  if #[cfg(target_arch="wasm32")]{
    fn get_config() -> Configuration{
        Configuration::new()
    }
  } else {
    use crate::engine::config::make_cli_parser;
    fn get_config() -> Configuration{
        let cli_args = &make_cli_parser().get_matches();
        Configuration::new(cli_args)
    }
  }
}
