use std::io;

fn main() -> io::Result<()> {
    if cfg!(target_os = "windows") && !cfg!(test) {
        let mut res = winres::WindowsResource::new();
        res.set_icon("src/icon.ico")
            .set("InternalName", "rust-pipes.exe");
        res.compile()?;
    }

    Ok(())
}
