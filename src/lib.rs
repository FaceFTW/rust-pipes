#![allow(special_module_name)]
cfg_if::cfg_if! {
    if #[cfg(target_arch="wasm32")]{
        mod main;
        mod config;
        mod util;
        mod world;

        // Entry point for wasm
        use wasm_bindgen::prelude::*;
        #[wasm_bindgen(start)]
        pub async fn start() -> Result<(), JsValue> {
            console_log::init_with_level(log::Level::Debug).unwrap();

            use log::info;
            info!("Logging works!");

            std::panic::set_hook(Box::new(console_error_panic_hook::hook));
            main::real_main().await;
            Ok(())
        }

    }
}
