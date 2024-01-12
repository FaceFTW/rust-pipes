# rust-pipes :crab::computer:

A remake of the classic Windows 98 Screensaver in Rust. Built to be resilient, configurable, and cross-platform (including for Web targets)

This project is still in development.

Web Development: https://pipes.faceftw.dev

## Building/Testing

This project uses cargo for the standard Rust project build cycle. You can do the following tasks with cargo as follows:

| Step              | Command               |
| ----------------- | --------------------- |
| Building          | `cargo build`         |
| Running (No Args) | `cargo run`           |
| Running (w/ Args) | `cargo run -- <args>` |
| Testing           | `cargo test`          |

### The `wasm` Target

This project also has a WASM target that is used for the web version, and is compiled differently from the standard version (i.e. lack of argument parsing).

```shell
cd ./web
npm install		# If cloning for the first time
npx wasm-pack build ".." --target web --out-name web --out-dir web/pkg
npm run serve
```

## Source/Reference Materials

I've used these resources to help build the project. Without them, this project would likely take forever to figure out how to complete.

- A lot of the original logic/design comes from the original source code for the screensaver in the [Microsoft Windows NT 4.0 SDK](https://winworldpc.com/download/3d03c2ad-c2ad-18c3-9a11-c3a4e284a2ef) found under `MSTOOLS\SAMPLES\OPENGL\SCRSAVE`
- [1j01's JS implementation](https://github.com/1j01/pipes) has also been instrumental to help with the translation of the original code written in "Microsoft C++ Dialect"

I'd also like to give a shoutout to these crates that I used that sped up the development of the project:

- [asny/three-d](https://github.com/asny/three-d) is an excellent crate for doing Graphics/OpenGL programming in Rust. It provides a great foundation for development regardless of the abstraction level you want to work at with great documentation and examples to accompany it. The developer is also very responsive and even [fixed a bug that affected this project](https://github.com/asny/three-d/issues/402)!
