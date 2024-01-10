#![allow(special_module_name)]
cfg_if::cfg_if! {
    if #[cfg(target_arch="wasm32")]{
        mod engine;

        // Entry point for wasm
        use wasm_bindgen::prelude::*;
        #[wasm_bindgen(start)]
        pub async fn start() -> Result<(), JsValue> {
            std::panic::set_hook(Box::new(console_error_panic_hook::hook));
            engine::real_main();
            Ok(())
        }
    }
}
mod engine;

pub fn entry_point() {
    engine::real_main();
}
