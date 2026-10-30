//  Server program options, implementation
//
//  Copyright (C) 2009, 2010 Tim Blechmann, 2026 Christof Ressi
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#include "SC_ServerProgramOptions.hpp"
#include "SC_Version.hpp"
#include "clz.h"

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp> // for physical_concurrency()

#ifdef _WIN32
#    define PATH_SEPARATOR ";"
#else
#    define PATH_SEPARATOR ":"
#endif

bool ServerProgramOptions::parse(int argc, char* argv[]) {
#ifdef NO_LIBSNDFILE
    static constexpr auto serverName = "supernova";
#else
    static constexpr auto serverName = "scsynth";
#endif

    namespace po = boost::program_options;

    // prepare options
    po::options_description allOptions;

    po::options_description generalOptions("general options");
    // clang-format off
    generalOptions.add_options()
        ("help,h", "show this help")
        ("version,v", "print the supercollider version and exit")
        ("verbose,V", po::value(&mVerbosity)->default_value(mVerbosity), "verbosity: 0 is normal behaviour\n"
         "-1 suppresses informational messages\n"
         "-2 suppresses informational and many error messages, as well as messages from Poll.")
        ;
    // clang-format on

    po::options_description synthOptions("synth options");
    // clang-format off
    synthOptions.add_options()
        ("block-size,z", po::value(&mBufLength)->default_value(mBufLength), "audio block size")
        ("control-buses,c", po::value(&mNumControlBusChannels)->default_value(mNumControlBusChannels),
         "number of control buses")
        ("audio-buses,a", po::value(&mNumAudioBusChannels)->default_value(mNumAudioBusChannels),
         "number of audio buses")
        ("buffers,b", po::value(&mNumBuffers)->default_value(mNumBuffers), "number of sound buffers")
        ("max-nodes,n", po::value(&mMaxNodes)->default_value(mMaxNodes), "maximum number of server nodes")
        ("max-synthdefs,d", po::value(&mMaxGraphDefs)->default_value(mMaxGraphDefs), "maximum number of synthdefs")
        ("wires,w", po::value(&mMaxWireBufs)->default_value(mMaxWireBufs), "number of wire buffers")
        ("randgens,r", po::value(&mNumRGens)->default_value(mNumRGens), "number of random number generators")
        ("rt-memory,m", po::value(&mRealTimeMemorySize)->default_value(mRealTimeMemorySize),
         "size of real-time memory pool in kilobytes")
        ("load-synthdefs,D", po::value(&mLoadGraphDefs)->default_value(mLoadGraphDefs), "load synthdefs? (0 or 1)")
        ("ugen-search-path,U", po::value(&mUGenPluginPathString), "A list of paths separated by '" PATH_SEPARATOR "'.\n"
         "If specified, standard paths are NOT searched for plugins.\n")
        ("restricted-path,P", po::value(&mRestrictedPathString), "if specified, prevents file-accessing OSC commands "
         "from accessing files outside <restricted-path>")
        ("nrt,N", po::value<std::vector<std::string>>()->multitoken(), "Non-realtime synthesis. Arguments:\n"
         "<cmd-filename> <input-filename> <output-filename> <sample-rate> <header-format> <sample-format>.\n"
         "Pass '_' as <input-filename> if you don't need any sound input.")
        ;
    // clang-format on

    // networking options
    po::options_description networkingOptions("networking options");
    // clang-format off
    networkingOptions.add_options()
        ("udp-port,u", po::value<int>(), "UDP port (1-65535). Set to 0 to bind to a free port.")
        ("tcp-port,t", po::value<int>(), "TCP port (1-65535). Set to 0 to bind to a free port.")
        ("bind-address,B", po::value(&mBindAddressString)->default_value(mBindAddressString),
         "Bind the UDP or TCP socket to this address.\n"
         "Set to 0.0.0.0 to listen on all interfaces.")
        ("max-logins,l", po::value(&mMaxLogins)->default_value(mMaxLogins), "maximum number of clients that "
         "can be registered for notifications.")
        ("password,p", po::value(&mPasswordString)->default_value(""), "An optional TCP session password.\n"
         "UDP ports never require passwords, so for security use TCP.")
        ("rendezvous,R", po::value(&mRendezvous)->default_value(mRendezvous), "publish to Rendezvous? (0 or 1)")
        ;
    // clang-format on

    po::options_description audioOptions("audio options");
    // clang-format off
    audioOptions.add_options()
        ("hardware-buffer-size,Z", po::value(&mPreferredHardwareBufferFrameSize),
         "Hardware buffer size. If not specified, uses the audio driver's default.")
        ("samplerate,S", po::value(&mPreferredSampleRate),
         "Hardware sample rate. If not specified, uses the audio driver's default.")
        ("inchannels,i", po::value(&mNumInputBusChannels)->default_value(mNumInputBusChannels),
         "number of input channels")
        ("outchannels,o", po::value(&mNumOutputBusChannels)->default_value(mNumOutputBusChannels),
         "number of output channels")
        ("hardware-device-name,H", po::value<std::vector<std::string>>()->multitoken(),
         "hardware device name. You can provide up to two names:\n"
         "-H <name>: uses the same device for input and output.\n"
         "-H <input> <output>: uses different devices for input and output.\n"
         "If the option is omitted, the Server uses the audio driver's default device(s).")
#ifdef __APPLE__
        ("input-streams,I", po::value(&mInputStreams), "input streams enabled")
        ("output-streams,O", po::value(&mInputStreams), "output streams enabled")
        ("safety-clip-threshold,s", po::value(&mSafetyClipThreshold)->default_value(mSafetyClipThreshold),
         "absolute amplitude value outputs will be clipped to.\n"
         "Set to <= 0 or inf to completely disable clipping.")
#endif
        ;
    // clang-format on

    po::options_description advancedOptions("advanced options");
    // clang-format off
    advancedOptions.add_options()
        ("memory-locking,L", "enable memory locking")
#ifdef SUPERNOVA
        ("use-system-clock,C", po::value(&mUseSystemClock)->default_value(mUseSystemClock),
         "type of clock: 0 = sample clock, 1 = system clock")
        ("threads,T", po::value(&mNumThreads)->default_value(boost::thread::physical_concurrency()),
         "number of audio threads")
        ("thread-pinning,y", po::value(&mThreadPinning)->default_value(mThreadPinning), "pin threads to CPU cores")
#endif
        ;
    // clang-format on

#ifdef SC_BELA
    po::options_description belaOptions("bela options");
    // clang-format off
    audioOptions.add_options()
        ("analog-inchannels,J", po::value(&mBelaAnalogInputChannels)->default_value(mBelaAnalogInputChannels),
         "analog input channels")
        ("analog-outchannels,K", po::value(&mBelaAnalogOutputChannels)->default_value(mBelaAnalogOutputChannels),
         "analog output channels")
        ("digital-channels,G", po::value(&mBelaDigitalChannels)->default_value(mBelaDigitalChannels), "digital channels")
        ("headphone-level,Q", po::value(&mBelaHeadphoneLevel)->default_value(mBelaHeadphoneLevel),
         "headphone level in dB (max: 0dB, min: -63.5dB)")
        ("pga-gain-left,X", po::value(&mBelaPgaGainLeft)->default_value(mBelaPgaGainLeft), "left PGA gain in dB")
        ("pga-gain-right,Y", po::value(&mBelaPgaGainRight)->default_value(mBelaPgaGainRight), "right PGA gain in dB")
        ("speaker-mute,A", po::value(&mBelaSpeakerMuted)->default_value(mBelaSpeakerMuted), "mute speakers")
        ("dac-level,x", po::value(&mBelaDacLevel)->default_value(mBelaDacLevel), "DAC level in dB")
        ("adc-level,y", po::value(&mBelaAdcLevel)->default_value(mBelaAdcLevel), "ADC level in dB")
        ("mux-channels,g", po::value(&mBelaNumMuxChannels)->default_value(mBelaNumMuxChannels), "multiplexer channels")
        ("pru-id,T", po::value(&mBelaPru)->default_value(mBelaPru), "PRU id")
        ("scope-channels,E", po::value(&mBelaMaxScopeChannels)->default_value(mBelaMaxScopeChannels), "max. oscilloscope channels")
        ;
    // clang-format on

#endif

    // add the options in the desired order
    allOptions.add(generalOptions);
    allOptions.add(audioOptions);
    allOptions.add(synthOptions);
    allOptions.add(networkingOptions);
    allOptions.add(advancedOptions);
#ifdef SC_BELA
    allOptions.add(belaOptions);
#endif

    // parse options
    po::variables_map vm;

    try {
        // disallow option guessing
        using namespace po::command_line_style;
        auto style = default_style & ~allow_guessing;

        po::store(po::parse_command_line(argc, argv, allOptions, style), vm);
    } catch (po::error const& e) { throw std::runtime_error(e.what()); }

    po::notify(vm);

    // some options need special handling

    // -h/--help
    if (vm.count("help")) {
        std::cout << allOptions << std::endl << std::endl;
        std::cout << "To quit, send a 'quit' command via UDP or TCP, or press ctrl-C." << std::endl;
        return false;
    }

    // -v/--version
    if (vm.count("version")) {
        std::cout << serverName << " " << SC_VersionString() << " (" << SC_BuildString() << ")" << std::endl;
        return false;
    }

    // -u/--udp-port
    if (vm.count("udp-port"))
        mUdpPort = vm["udp-port"].as<int>();

    // -t/--tcp-port
    if (vm.count("tcp-port"))
        mTcpPort = vm["tcp-port"].as<int>();

    // -B/--bind-address
    try {
        mBindAddress = boost::asio::ip::make_address(mBindAddressString);
    } catch (boost::system::system_error const& e) {
        throw std::runtime_error("Cannot parse '" + mBindAddressString + "' as a valid IP address.");
    };

    // -L/--memory-locking
    if (auto memoryLocking = vm.count("memory-locking") > 0) {
#if (_POSIX_MEMLOCK - 0) >= 200112L
        mMemoryLocking = true;
#else
        std::cout << "ignoring -L/--memory-locking option: memory locking not supported on this platform" << std::endl;
#endif
    }

    // -H/--hardware-device-name
    if (vm.count("hardware-device-name") > 0)
        mHardwareDeviceName = vm["hardware-device-name"].as<std::vector<std::string>>();

    // -N/--nrt
    mRealTime = vm.count("nrt") == 0;
    if (!mRealTime) {
        auto const& nrt_options = vm["nrt"].as<std::vector<std::string>>();
        if (nrt_options.size() != 6) {
            throw std::runtime_error("wrong number of arguments  for --nrt/-N option");
        }

#ifdef NO_LIBSNDFILE
        std::stringstream ss;
        ss << "NRT mode not supported: " << serverName << " compiled without libsndfile";
        throw std::runtime_error(ss.str());
#endif

        mCommandFile = nrt_options[0];
        mInputFile = nrt_options[1];
        mOutputFile = nrt_options[2];
        // NOTE: the sample rate provided with -N beats the one provided with -S!
        mPreferredSampleRate = boost::lexical_cast<uint32_t>(nrt_options[3]);
        mHeaderFormat = nrt_options[4];
        mSampleFormat = nrt_options[5];

        // assign the strings to the actual WorldOption arguments
        mNonRealTimeCmdFilename = mCommandFile != "_" ? mCommandFile.c_str() : nullptr;
        mNonRealTimeInputFilename = mInputFile != "_" ? mInputFile.c_str() : nullptr;
        mNonRealTimeOutputFilename = mOutputFile.c_str();
        mNonRealTimeOutputHeaderFormat = mHeaderFormat.c_str();
        mNonRealTimeOutputSampleFormat = mSampleFormat.c_str();
    }

    validateOptions();

    // some options are calculated from others (*after* they have been validated)
    if (mRealTime) {
        assert(mUdpPort.has_value() || mTcpPort.has_value());
        // use the port number as our shared memory ID
        mSharedMemoryID = mUdpPort.has_value() ? mUdpPort.value() : mTcpPort.has_value();
    }

    // finally, assign the strings to the actual WorldOption arguments
    if (!mPasswordString.empty())
        mPassword = mPasswordString.c_str();
    if (!mUGenPluginPathString.empty())
        mUGensPluginPath = mUGenPluginPathString.c_str();
    if (!mRestrictedPathString.empty())
        mRestrictedPath = mRestrictedPathString.c_str();
#ifdef __APPLE__
    if (!mInputStreams.empty())
        mInputStreamsEnabled = mInputStreams.c_str();
    if (!mOutputStreams.empty())
        mOutputStreamsEnabled = mOutputStreams.c_str();
#endif
    // The user can specify up to two hardware device names,
    // which gives us three different scenarios:
    // 1. two device names -> use different input and output devices
    // 2. one device name -> use the same device for input and output
    // 3. no device name -> let the Server pick a default device.
    if (mHardwareDeviceName.size() > 0) {
        mInDeviceName = mHardwareDeviceName[0].c_str();
        if (mHardwareDeviceName.size() > 1) {
            mOutDeviceName = mHardwareDeviceName[1].c_str();
        } else {
            mOutDeviceName = mInDeviceName;
        }
    }

    return true;
}

void ServerProgramOptions::validateOptions() {
    // check port numbers
    if (auto value = mUdpPort.value_or(0); value < 0 || value > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("UDP port number (" + std::to_string(value) + ") out of range");
    }
    if (auto value = mTcpPort.value_or(0); value < 0 || value > std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("TCP port number (" + std::to_string(value) + ") out of range");
    }

    // block size must be a power of two
    if (!ISPOWEROFTWO(mBufLength)) {
        throw std::runtime_error("Block size (" + std::to_string(mBufLength) + ") not a power of two.");
    }

    // hardware buffer size (if provided) must be a power of two.
    if (auto wantedBufferSize = mPreferredHardwareBufferFrameSize;
        wantedBufferSize > 0 && !ISPOWEROFTWO(wantedBufferSize)) {
        mPreferredHardwareBufferFrameSize = NEXTPOWEROFTWO(wantedBufferSize);
        std::cout << "Hardware buffer size (" << wantedBufferSize << ") not a power of two! "
                  << "Setting to " << mPreferredHardwareBufferFrameSize << "." << std::endl;
    }
    // hardware buffer size (if provided) must be >= block size
    if (auto wantedBufferSize = mPreferredHardwareBufferFrameSize;
        wantedBufferSize > 0 && wantedBufferSize < mBufLength) {
        mPreferredHardwareBufferFrameSize = mBufLength;
        std::cout << "Hardware buffer size (" << wantedBufferSize << ") must not be smaller than block size! "
                  << "Setting to " << mPreferredHardwareBufferFrameSize << "." << std::endl;
    }

    // in non-realtime mode the sample rate cannot be zero
    if (!mRealTime && mPreferredSampleRate == 0) {
        throw std::runtime_error("Bad sample rate (" + std::to_string(mPreferredSampleRate) + ") for NRT synthesis.");
    }

    // realtime mode requires a UDP and/or TCP port
    if (mRealTime && !mUdpPort.has_value() && !mTcpPort.has_value()) {
        throw std::runtime_error("Realtime mode requires a UDP and/or TCP port. "
                                 "See the -u/--udp-port and -t/--tcp-port options.");
    }

    // check if we have enough audio bus channels
    if (mNumInputBusChannels + mNumOutputBusChannels > mNumAudioBusChannels) {
        std::stringstream ss;
        ss << "Too few audio bus channels (" << mNumAudioBusChannels << ") for the given number of inputs ("
           << mNumInputBusChannels << ") and outputs (" << mNumOutputBusChannels << ")";
        throw std::runtime_error(ss.str());
    }

    // cannot use more than 2 hardware device names
    if (mHardwareDeviceName.size() > 2) {
        auto extra = mHardwareDeviceName.size() - 2;
        std::cout << "Ignoring extra " << extra << " hardware device names." << std::endl;
    }
}
