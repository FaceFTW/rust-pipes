use std::io;

#[cfg(not(target_os = "windows"))]
fn main() -> io::Result<()> {
    Ok(())
}

#[cfg(target_os = "windows")]
fn main() -> io::Result<()> {
    //Only perform Windows Resource application if we are targeting Windows
    //See https://github.com/rust-lang/cargo/issues/14881#issuecomment-2510440862
    let target_os =
        std::env::var("CARGO_CFG_TARGET_OS").expect("Cargo did not set the vars we expected!");
    if target_os == "windows" {
        let mut res = winres::WindowsResource::new();
        res.set_icon("src/icon.ico")
            .set("InternalName", "rust-pipes.exe");
        res.compile()?;
    }

    Ok(())
}
