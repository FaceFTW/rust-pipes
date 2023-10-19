use clap::{command, Arg, ArgAction, ArgGroup, Command};

pub fn make_cli_parser() -> Command {
    command!()
        .group(ArgGroup::new("sim").required(false).multiple(true))
        .next_help_heading("Simulation Options")
        .args([
            Arg::new("single-run").action(ArgAction::SetTrue),
            Arg::new("max-pipes")
                .short('p')
                .value_parser(clap::value_parser!(u8).range(1..))
                .default_value("8"),
            Arg::new("max-bounds")
                .num_args(3)
                .value_parser(clap::value_parser!(u8).range(8..))
                .value_names(["x", "y", "z"])
                .default_values(["20", "20", "20"]),
            Arg::new("turn-chance").value_parser(clap::value_parser!(f32)),
        ])
        .group(ArgGroup::new("render").required(false).multiple(true))
        .next_help_heading("Rendering Options")
        .args([
            Arg::new("resolution")
                .value_parser(clap::value_parser!(u32).range(8..))
                .default_value("16")
                .long("poly-resoluition"),
            Arg::new(""),
        ])
}

#[cfg(test)]
mod tests {
    use super::make_cli_parser;

    #[test]
    pub fn cli_single_run_flag_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--single-run"]);
        assert!(match1.contains_id("single-run"));
        assert_eq!(match1.get_flag("single-run"), true);

        //recreate because of rust semantics (also good for ensuring clean state)
        let cli_parser = make_cli_parser();
        let match2 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match2.contains_id("single-run"));
        assert_eq!(match2.get_flag("single-run"), false);
    }

    #[test]
    pub fn cli_max_pipe_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-pipes"));
        let val1: u8 = *match1
            .get_one("max-pipes")
            .expect("Default value should have been configured");
        assert_eq!(val1, 8);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-pipes", "100"]);
        assert!(match1.contains_id("max-pipes"));
        let val1: u8 = *match1
            .get_one::<u8>("max-pipes")
            .expect("Default value should have been configured");
        assert_eq!(val1, 100);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "-p", "50"]);
        assert!(match1.contains_id("max-pipes"));
        let val1: u8 = *match1
            .get_one("max-pipes")
            .expect("Default value should have been configured");
        assert_eq!(val1, 50);
    }

    #[test]
    pub fn cli_max_bound_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-bounds"));
        let val1 = match1
            .get_many::<u8>("max-bounds")
            .expect("Default value should have been configured")
            .copied()
            .collect::<Vec<u8>>();
        assert_eq!(val1, vec![20, 20, 20]);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-bounds", "10", "11", "12"]);
        assert!(match1.contains_id("max-bounds"));
        let val1 = match1
            .get_many::<u8>("max-bounds")
            .expect("Default value should have been configured")
            .copied()
            .collect::<Vec<u8>>();
        assert_eq!(val1, vec![10, 11, 12]);
    }
}
