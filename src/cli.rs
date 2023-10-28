use clap::{command, Arg, ArgAction, ArgGroup, Command};

pub fn make_cli_parser() -> Command {
    command!()
        .group(ArgGroup::new("exec").required(false).multiple(true))
        .next_help_heading("Execution Options")
        .args([Arg::new("rng-seed").long("seed").value_name("seed").help(
            "The seed input used by the random number generator for pipe generation.\n The state of the random number generator is not reset between simulations.\n Seeds can be alphanumeric.",
        )])
        .group(ArgGroup::new("sim").required(false).multiple(true))
        .next_help_heading("Simulation Options")
        .args([
            Arg::new("single-run")
                .long("single-run")
                .default_value("false")
                .action(ArgAction::SetTrue)
                .help("If the simulation should be run only once"),
            //================ --max-pipes ================
            Arg::new("max-pipes")
                .short('p')
                .long("max-pipes")
                .value_parser(clap::value_parser!(u8).range(1..))
                .default_value("8")
                .help("Maximum number of pipes to create in each simulation run"),
            //================ --max-bounds ================
            Arg::new("max-bounds")
                .long("max-bounds")
                .num_args(3)
                .value_parser(clap::value_parser!(u8).range(8..))
                .value_names(["x", "y", "z"])
                .default_values(["20", "20", "20"]),
            //================ --turn-chance ================
            Arg::new("turn-chance")
                .long("turn-chance")
                .value_parser(clap::value_parser!(f32))
                .default_value("0.3"),
            //================ --max-gen-time ================
            Arg::new("max-gen-time")
                .long("max-gen-time")
                .value_parser(clap::value_parser!(u32))
                .default_value("15"),
            //================ --max-freeze-time ================
            Arg::new("max-freeze-time")
                .long("max-freeze-time")
                .value_parser(clap::value_parser!(u32))
                .default_value("10")
                .conflicts_with("single-run"),
        ])
        .group(ArgGroup::new("render").required(false).multiple(true))
        .next_help_heading("Rendering Options")
        .args([
            Arg::new("resolution")
                .value_parser(clap::value_parser!(u32).range(8..))
                .default_value("16")
                .long("poly-subdivisions"),
            Arg::new("original-colors")
                .long("original-colors")
                .default_value("false")
                .value_name("true/false")
                .value_parser(clap::value_parser!(bool))
                .help("If we should only render using the colors defined in the original screensaver."),
            Arg::new("bg-color")
                .long("bg-color")
                .num_args(3)
                .value_names(["red","green","blue"])
                .default_values(["0","0","0"])
                .value_parser(clap::value_parser!(u8))
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

    #[test]
    pub fn cli_max_gen_time_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-gen-time"));
        let val1: u32 = *match1
            .get_one("max-gen-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 15);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-gen-time", "100"]);
        assert!(match1.contains_id("max-gen-time"));
        let val1: u32 = *match1
            .get_one::<u32>("max-gen-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 100);
    }

    #[test]
    pub fn cli_max_freeze_time_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("max-freeze-time"));
        let val1: u32 = *match1
            .get_one("max-freeze-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 10);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--max-freeze-time", "100"]);
        assert!(match1.contains_id("max-freeze-time"));
        let val1: u32 = *match1
            .get_one::<u32>("max-freeze-time")
            .expect("Default value should have been configured");
        assert_eq!(val1, 100);
    }

    #[test]
    pub fn cli_turn_chance_option_tests() {
        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog"]);
        assert!(match1.contains_id("turn-chance"));
        let val1: f32 = *match1
            .get_one("turn-chance")
            .expect("Default value should have been configured");
        assert_eq!(val1, 0.3);

        let cli_parser = make_cli_parser();
        let match1 = cli_parser.get_matches_from(vec!["prog", "--turn-chance", "0.75"]);
        assert!(match1.contains_id("turn-chance"));
        let val1: f32 = *match1
            .get_one::<f32>("turn-chance")
            .expect("Default value should have been configured");
        assert_eq!(val1, 0.75);
    }
}
