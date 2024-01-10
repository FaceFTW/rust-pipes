#![windows_subsystem = "windows"]
///This is an entry point for standard executables, basically
/// invokes the engine facade to do the actual thing.
fn main() {
    rust_pipes_lib::entry_point();
}
