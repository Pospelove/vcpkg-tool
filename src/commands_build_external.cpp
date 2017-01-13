#include "vcpkg_Commands.h"
#include "vcpkg_System.h"
#include "vcpkg_Environment.h"
#include "vcpkg_Input.h"
#include "vcpkg.h"

namespace vcpkg::Commands
{
    void build_external_command(const vcpkg_cmd_arguments& args, const vcpkg_paths& paths, const triplet& default_target_triplet)
    {
        static const std::string example = Commands::Helpers::create_example_string(R"(build_external zlib2 C:\path\to\dir\with\controlfile\)");
        args.check_exact_arg_count(2, example);

        expected<package_spec> maybe_current_spec = package_spec::from_string(args.command_arguments[0], default_target_triplet);
        if (auto spec = maybe_current_spec.get())
        {
            Input::check_triplet(spec->target_triplet(), paths);
            Environment::ensure_utilities_on_path(paths);
            const fs::path port_dir = args.command_arguments.at(1);
            const expected<SourceParagraph> maybe_spgh = try_load_port(port_dir);
            if (auto spgh = maybe_spgh.get())
            {
                Commands::build_internal(*spgh, *spec, paths, port_dir);
                exit(EXIT_SUCCESS);
            }
        }

        System::println(System::color::error, "Error: %s: %s", maybe_current_spec.error_code().message(), args.command_arguments[0]);
        Commands::Helpers::print_example(Strings::format("%s zlib:x64-windows", args.command));
        exit(EXIT_FAILURE);
    }
}
