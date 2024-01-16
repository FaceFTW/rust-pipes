///Literally just contains code/data to load and render the funny teapot.
use three_d_asset::{io::RawAssets, TriMesh};

///You could theoretically replace this with any viable model
/// as long as it is OBJ format. Scaling is a whole other issue however...
const TEAPOT_OBJ: &[u8] = include_bytes!("../teapot.obj");

///Method to deserialize the OBJ file inlined in the executable.
/// This is executed once by the engine so that it we don't
/// need to "recache" the object everytime we want to render the
/// teapot.
pub(crate) fn teapot_mesh() -> TriMesh {
    let mut assets = RawAssets::new();
    assets.insert("teapot.obj", Vec::from(TEAPOT_OBJ));
    let obj: TriMesh = assets.deserialize("teapot.obj").unwrap();

    obj
}

#[cfg(test)]
mod tests {
    use super::teapot_mesh;

    #[test]
    fn test_teapot_mesh_deserialized_as_valid_trimesh() {
        let mesh = teapot_mesh();

        mesh.validate().expect("Mesh Failed to Validate");

        //Assert that the inlined OBJ data has been loaded by the lib.
        assert!(!mesh.indices.is_empty());
        assert!(mesh
            .normals
            .clone()
            .is_some_and(|val| !&val.clone().is_empty()));
        assert!(mesh.uvs.clone().is_some_and(|val| !&val.clone().is_empty()));
        assert!(mesh.vertex_count() > 0);
    }
}
