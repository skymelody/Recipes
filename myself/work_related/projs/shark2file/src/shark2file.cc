//
// Created by skymelody on 2019/12/21.
//

#include <string>
#include <vector>
#include <thread>


#include "Shark2File/Rtp/RtpPacketHandlerFactory.h"
#include "Shark2File/Rtp/RtpPacketFilter.h"
#include "Shark2File/Common/Utils.h"
#include "Shark2File/Common/PacketHandlerFactory.h"
#include "Shark2File/Common/Program.h"
#include "Shark2File/Common/Log.h"


using namespace std;

void ShowUsage() {
    LOG("Usage:"
           CLRF
           "-in    + input path\t\t\t: specify input pcap file path"
           CLRF
           "-out   + output path\t\t\t: specify output directory, the default output path is either input file name or captured file name"
           CLRF
           "-i     + net card\t\t\t: specify which net card to capture"
           CLRF
           "-ws    + wireshark path\t\t\t: if use wireshark to capture packet, specify wireshark installation path"
           CLRF
           "-wsout + wireshark output path\t\t: if use wireshark, specify captured packets' output path"
           CLRF
	       "\t\t\t\t\t  the default output path is current time stample"
           CLRF
	       "-D\t\t\t\t\t: show capturable net cards, specify wireshark path before this option"
           CLRF
           CLRF
           "-pd    + payload type1 + avtype1"
           CLRF
           "       + payload type2 + avtype2"
           CLRF
           "       + ..."
		   CLRF
		   "\t\t\t\t\t: extract AV data from rtp packets, avtype is either 'a' or 'v'"
	       CLRF);
}


bool ParseCmdList(const std::vector<std::string>& args) {
	if (args.empty()) {
		return false;
	}

	if (args[0] == "-h") {
		return false;
	}

	int i = 0;
	int nargs = args.size();
	while (i < nargs) {
		const std::string& arg = args[i];
		if (arg == "-in"  ||
		    arg == "-out" ||
		    arg == "-i"   ||
		    arg == "-ws"  ||
		    arg == "-wsout") {
			if (i+1 >= nargs) {
				return false;
			}
			i += 2;
		}
		else if (arg == "-D") {
			++i;
		}
		else if (arg == "-pd") {
			int j = 0;
			while (++i < nargs && args[i][0] != '-') ++j;
			if (j % 2 != 0 || j < 2) {
				return false;
			}

			continue;
		}
		else {
			return false;
		}
	}

	return true;
}


int main(int argc, const char** argv) {

	std::vector<std::string> args;
	for (int i = 1; i < argc; ++i) {
		args.emplace_back(argv[i]);
	}

	if (!ParseCmdList(args)) {
		ShowUsage();
		return 0;
	}

	Options options;
	if (!options.SetupOptions(args)) {
		ShowUsage();
		return 0;
	}

	if (options.UseWireShark()) {
		Program program(options.WireSharkPath());
		if (options.ListNetInterface()) {
			program.AddArg("-D");
			program.Execute();
			return 0;
		}
		program.AddArg("-Pw"+options.CapturePacketOutPath()+".pcap");
		program.AddArg("-Fpcap");
		if (!options.NetInterface().empty()) {
			program.AddArg("-i"+options.NetInterface());
		}
		program.Execute();
	}
	std::shared_ptr<PacketHandlerFactory> factory(new RtpPacketHandlerFactory());
	std::shared_ptr<PacketFilter> filter(new RtpPacketFilter());

	std::thread t(StartHandler, factory, filter, options);
	t.join();

    return 0;
}
