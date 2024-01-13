use three_d_asset::{
    io::{Deserialize, RawAssets},
    TriMesh,
};

///Literally just contains data to render the funny teapot.

const TEAPOT_OBJ: &[u8] = include_bytes!("../teapot.obj");

pub(crate) fn teapot_mesh() -> TriMesh {
    let mut assets = RawAssets::new();
    assets.insert("teapot.obj", Vec::from(TEAPOT_OBJ));

    let mut obj: TriMesh = assets.deserialize("teapot.obj").unwrap();

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
