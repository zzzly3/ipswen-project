#line 9 "client/cmds.Y"
{ "quit", "", "Quit the client", 1 },
#line 10 "client/cmds.Y"
{ "exit", "", "Exit the client", 1 },
#line 11 "client/cmds.Y"
{ "help", "", "Description of the help system", 1 },
#line 609 "nest/config.Y"
{ "show", "...", "Show status information", 0 },
#line 611 "nest/config.Y"
{ "show status", "", "Show router status", 1 },
#line 614 "nest/config.Y"
{ "show memory", "", "Show memory usage", 1 },
#line 617 "nest/config.Y"
{ "show protocols", "[<protocol> | \"<pattern>\"]", "Show routing protocols", 1 },
#line 620 "nest/config.Y"
{ "show protocols all", "[<protocol> | \"<pattern>\"]", "Show routing protocol details", 1 },
#line 628 "nest/config.Y"
{ "show interfaces", "", "Show network interfaces", 1 },
#line 631 "nest/config.Y"
{ "show interfaces summary", "", "Show summary of network interfaces", 1 },
#line 634 "nest/config.Y"
{ "show route", "...", "Show routing table", 0 },
#line 635 "nest/config.Y"
{ "show route", "[<prefix>|for <prefix>|for <ip>|in <prefix>] [table <t>] [(import|export) table <p>.<c>] [filter <f>|where <cond>] [all] [primary] [filtered] [(export|preexport|noexport) <p>] [protocol <p>] [stats|count]", "Show routing table", 1 },
#line 825 "nest/config.Y"
{ "show symbols", "...", "Show all known symbolic names", 0 },
#line 826 "nest/config.Y"
{ "show symbols", "[table|filter|function|protocol|template|<symbol>]", "Show all known symbolic names", 1 },
#line 842 "nest/config.Y"
{ "dump", "...", "Dump debugging information", 0 },
#line 843 "nest/config.Y"
{ "dump resources", "", "Dump all allocated resource", 1 },
#line 845 "nest/config.Y"
{ "dump sockets", "", "Dump open sockets", 1 },
#line 847 "nest/config.Y"
{ "dump events", "", "Dump event log", 1 },
#line 849 "nest/config.Y"
{ "dump interfaces", "", "Dump interface information", 1 },
#line 851 "nest/config.Y"
{ "dump neighbors", "", "Dump neighbor cache", 1 },
#line 853 "nest/config.Y"
{ "dump attributes", "", "Dump attribute cache", 1 },
#line 855 "nest/config.Y"
{ "dump routes", "", "Dump routing table", 1 },
#line 857 "nest/config.Y"
{ "dump protocols", "", "Dump protocol information", 1 },
#line 859 "nest/config.Y"
{ "dump filter all", "", "Dump all filters in linearized form", 1 },
#line 862 "nest/config.Y"
{ "eval", "<expr>", "Evaluate an expression", 1 },
#line 865 "nest/config.Y"
{ "echo", "...", "Control echoing of log messages", 0 },
#line 866 "nest/config.Y"
{ "echo", "(all | off | { debug|trace|info|remote|warning|error|auth [, ...] }) [<buffer-size>]", "Control echoing of log messages", 1 },
#line 885 "nest/config.Y"
{ "disable", "(<protocol> | \"<pattern>\" | all) [message]", "Disable protocol", 1 },
#line 887 "nest/config.Y"
{ "enable", "(<protocol> | \"<pattern>\" | all) [message]", "Enable protocol", 1 },
#line 889 "nest/config.Y"
{ "restart", "(<protocol> | \"<pattern>\" | all) [message]", "Restart protocol", 1 },
#line 891 "nest/config.Y"
{ "reload", "<protocol> | \"<pattern>\" | all", "Reload protocol", 1 },
#line 893 "nest/config.Y"
{ "reload in", "<protocol> | \"<pattern>\" | all", "Reload protocol (just imported routes)", 1 },
#line 895 "nest/config.Y"
{ "reload out", "<protocol> | \"<pattern>\" | all", "Reload protocol (just exported routes)", 1 },
#line 898 "nest/config.Y"
{ "debug", "...", "Control protocol debugging via BIRD logs", 0 },
#line 899 "nest/config.Y"
{ "debug", "(<protocol> | <channel> | \"<pattern>\" | all) (all | off | { states|routes|filters|interfaces|events|packets [, ...] })", "Control protocol debugging via BIRD logs", 1 },
#line 907 "nest/config.Y"
{ "mrtdump", "...", "Control protocol debugging via MRTdump files", 0 },
#line 908 "nest/config.Y"
{ "mrtdump", "(<protocol> | \"<pattern>\" | all) (all | off | { states|messages [, ...] })", "Control protocol debugging via MRTdump format", 1 },
#line 911 "nest/config.Y"
{ "restrict", "", "Restrict current CLI session to safe commands", 1 },
#line 185 "proto/bfd/config.Y"
{ "show bfd", "...", "Show information about BFD protocol", 0 },
#line 186 "proto/bfd/config.Y"
{ "show bfd sessions", "[<name>]", "Show information about BFD sessions", 1 },
#line 171 "proto/babel/config.Y"
{ "show babel", "...", "Show information about Babel protocol", 0 },
#line 173 "proto/babel/config.Y"
{ "show babel interfaces", "[<name>] [\"<interface>\"]", "Show information about Babel interfaces", 1 },
#line 176 "proto/babel/config.Y"
{ "show babel neighbors", "[<name>] [\"<interface>\"]", "Show information about Babel neighbors", 1 },
#line 179 "proto/babel/config.Y"
{ "show babel entries", "[<name>]", "Show information about Babel prefix entries", 1 },
#line 182 "proto/babel/config.Y"
{ "show babel routes", "[<name>]", "Show information about Babel route entries", 1 },
#line 52 "proto/mrt/config.Y"
{ "mrt dump", "[table <name>|\"<pattern>\"] [to \"<file>\"] [filter <filter>|where <where filter>] ", "Save MRT Table Dump into a file", 0 },
#line 53 "proto/mrt/config.Y"
{ "mrt dump", "[table <name>|\"<pattern>\"] [to \"<file>\"] [filter <filter>|where <where filter>]", "Save mrt table dump v2 of table name <t> right now", 1 },
#line 513 "proto/ospf/config.Y"
{ "show ospf", "...", "Show information about OSPF protocol", 0 },
#line 514 "proto/ospf/config.Y"
{ "show ospf", "[<name>]", "Show information about OSPF protocol", 1 },
#line 517 "proto/ospf/config.Y"
{ "show ospf neighbors", "[<name>] [\"<interface>\"]", "Show information about OSPF neighbors", 1 },
#line 520 "proto/ospf/config.Y"
{ "show ospf interface", "[<name>] [\"<interface>\"]", "Show information about interface", 1 },
#line 523 "proto/ospf/config.Y"
{ "show ospf topology", "[all] [<name>]", "Show information about OSPF network topology", 0 },
#line 525 "proto/ospf/config.Y"
{ "show ospf topology", "[<name>]", "Show information about reachable OSPF network topology", 1 },
#line 528 "proto/ospf/config.Y"
{ "show ospf topology all", "[<name>]", "Show information about all OSPF network topology", 1 },
#line 531 "proto/ospf/config.Y"
{ "show ospf state", "[all] [<name>]", "Show information about OSPF network state", 0 },
#line 533 "proto/ospf/config.Y"
{ "show ospf state", "[<name>]", "Show information about reachable OSPF network state", 1 },
#line 536 "proto/ospf/config.Y"
{ "show ospf state all", "[<name>]", "Show information about all OSPF network state", 1 },
#line 539 "proto/ospf/config.Y"
{ "show ospf lsadb", "...", "Show content of OSPF LSA database", 0 },
#line 540 "proto/ospf/config.Y"
{ "show ospf lsadb", "[global | area <id> | link] [type <num>] [lsid <id>] [self | router <id>] [<proto>]", "Show content of OSPF LSA database", 1 },
#line 196 "proto/rip/config.Y"
{ "show rip", "...", "Show information about RIP protocol", 0 },
#line 198 "proto/rip/config.Y"
{ "show rip interfaces", "[<name>] [\"<interface>\"]", "Show information about RIP interfaces", 1 },
#line 201 "proto/rip/config.Y"
{ "show rip neighbors", "[<name>] [\"<interface>\"]", "Show information about RIP neighbors", 1 },
#line 160 "proto/static/config.Y"
{ "show static", "[<name>]", "Show details of static protocol", 1 },
#line 116 "sysdep/unix/config.Y"
{ "configure", "...", "Reload configuration", 0 },
#line 118 "sysdep/unix/config.Y"
{ "configure", "[\"<file>\"] [timeout [<sec>]]", "Reload configuration", 1 },
#line 121 "sysdep/unix/config.Y"
{ "configure soft", "[\"<file>\"] [timeout [<sec>]]", "Reload configuration and ignore changes in filters", 1 },
#line 125 "sysdep/unix/config.Y"
{ "configure timeout", "[<sec>]", "Reload configuration with undo timeout", 1 },
#line 126 "sysdep/unix/config.Y"
{ "configure soft timeout", "[<sec>]", "Reload configuration with undo timeout", 1 },
#line 128 "sysdep/unix/config.Y"
{ "configure confirm", "", "Confirm last configuration change - deactivate undo timeout", 1 },
#line 131 "sysdep/unix/config.Y"
{ "configure undo", "", "Undo last configuration change", 1 },
#line 134 "sysdep/unix/config.Y"
{ "configure status", "", "Show configuration status", 1 },
#line 137 "sysdep/unix/config.Y"
{ "configure check", "[\"<file>\"]", "Parse configuration and check its validity", 1 },
#line 140 "sysdep/unix/config.Y"
{ "down", "", "Shut the daemon down", 1 },
#line 143 "sysdep/unix/config.Y"
{ "graceful", "restart", "Shut the daemon down for graceful restart", 0 },
#line 145 "sysdep/unix/config.Y"
{ "graceful restart", "", "Shut the daemon down for graceful restart", 1 },
