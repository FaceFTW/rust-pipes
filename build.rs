use std::io;

#[cfg(all(target_os = "windows", not(target_arch = "wasm32")))]
fn main() -> io::Result<()> {
    if cfg!(target_os = "windows") && !cfg!(test) {
        let mut res = winres::WindowsResource::new();
        res.set_icon("src/icon.ico")
            .set("InternalName", "rust-pipes.exe");
        res.compile()?;
    }

    Ok(())
}

#[cfg(any(not(target_os = "windows"), target_arch = "wasm32"))]
fn main() -> io::Result<()> {
    Ok(())
}
