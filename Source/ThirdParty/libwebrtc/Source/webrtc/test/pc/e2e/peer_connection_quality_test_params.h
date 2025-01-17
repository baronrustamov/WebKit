/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_PC_E2E_PEER_CONNECTION_QUALITY_TEST_PARAMS_H_
#define TEST_PC_E2E_PEER_CONNECTION_QUALITY_TEST_PARAMS_H_

#include <memory>
#include <string>
#include <vector>

#include "api/async_resolver_factory.h"
#include "api/audio/audio_mixer.h"
#include "api/call/call_factory_interface.h"
#include "api/fec_controller.h"
#include "api/field_trials_view.h"
#include "api/rtc_event_log/rtc_event_log_factory_interface.h"
#include "api/task_queue/task_queue_factory.h"
#include "api/test/peerconnection_quality_test_fixture.h"
#include "api/transport/network_control.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "p2p/base/port_allocator.h"
#include "rtc_base/network.h"
#include "rtc_base/rtc_certificate_generator.h"
#include "rtc_base/ssl_certificate.h"
#include "rtc_base/thread.h"

namespace webrtc {
namespace webrtc_pc_e2e {

// Contains most part from PeerConnectionFactoryDependencies. Also all fields
// are optional and defaults will be provided by fixture implementation if
// any will be omitted.
//
// Separate class was introduced to clarify which components can be
// overridden. For example worker and signaling threads will be provided by
// fixture implementation. The same is applicable to the media engine. So user
// can override only some parts of media engine like video encoder/decoder
// factories.
struct PeerConnectionFactoryComponents {
  std::unique_ptr<TaskQueueFactory> task_queue_factory;
  std::unique_ptr<CallFactoryInterface> call_factory;
  std::unique_ptr<RtcEventLogFactoryInterface> event_log_factory;
  std::unique_ptr<FecControllerFactoryInterface> fec_controller_factory;
  std::unique_ptr<NetworkControllerFactoryInterface> network_controller_factory;
  std::unique_ptr<NetEqFactory> neteq_factory;

  // Will be passed to MediaEngineInterface, that will be used in
  // PeerConnectionFactory.
  std::unique_ptr<VideoEncoderFactory> video_encoder_factory;
  std::unique_ptr<VideoDecoderFactory> video_decoder_factory;

  std::unique_ptr<FieldTrialsView> trials;

  rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing;
  rtc::scoped_refptr<webrtc::AudioMixer> audio_mixer;
};

// Contains most parts from PeerConnectionDependencies. Also all fields are
// optional and defaults will be provided by fixture implementation if any
// will be omitted.
//
// Separate class was introduced to clarify which components can be
// overridden. For example observer, which is required to
// PeerConnectionDependencies, will be provided by fixture implementation,
// so client can't inject its own. Also only network manager can be overridden
// inside port allocator.
struct PeerConnectionComponents {
  PeerConnectionComponents(rtc::NetworkManager* network_manager,
                           rtc::PacketSocketFactory* packet_socket_factory)
      : network_manager(network_manager),
        packet_socket_factory(packet_socket_factory) {
    RTC_CHECK(network_manager);
  }

  rtc::NetworkManager* const network_manager;
  rtc::PacketSocketFactory* const packet_socket_factory;
  std::unique_ptr<webrtc::AsyncResolverFactory> async_resolver_factory;
  std::unique_ptr<rtc::RTCCertificateGeneratorInterface> cert_generator;
  std::unique_ptr<rtc::SSLCertificateVerifier> tls_cert_verifier;
  std::unique_ptr<IceTransportFactory> ice_transport_factory;
};

// Contains all components, that can be overridden in peer connection. Also
// has a network thread, that will be used to communicate with another peers.
struct InjectableComponents {
  InjectableComponents(rtc::Thread* network_thread,
                       rtc::NetworkManager* network_manager,
                       rtc::PacketSocketFactory* packet_socket_factory)
      : network_thread(network_thread),
        pcf_dependencies(std::make_unique<PeerConnectionFactoryComponents>()),
        pc_dependencies(
            std::make_unique<PeerConnectionComponents>(network_manager,
                                                       packet_socket_factory)) {
    RTC_CHECK(network_thread);
  }

  rtc::Thread* const network_thread;

  std::unique_ptr<PeerConnectionFactoryComponents> pcf_dependencies;
  std::unique_ptr<PeerConnectionComponents> pc_dependencies;
};

// Contains information about call media streams (up to 1 audio stream and
// unlimited amount of video streams) and rtc configuration, that will be used
// to set up peer connection.
struct Params {
  // Peer name. If empty - default one will be set by the fixture.
  absl::optional<std::string> name;
<<<<<<< HEAD
  // If `audio_config` is set audio stream will be configured
  absl::optional<PeerConnectionE2EQualityTestFixture::AudioConfig> audio_config;
  // Flags to set on `cricket::PortAllocator`. These flags will be added
  // to the default ones that are presented on the port allocator.
  uint32_t port_allocator_extra_flags = cricket::kDefaultPortAllocatorFlags;
  // If `rtc_event_log_path` is set, an RTCEventLog will be saved in that
=======
  // If |video_configs| is empty - no video should be added to the test call.
  std::vector<PeerConnectionE2EQualityTestFixture::VideoConfig> video_configs;
  // If |audio_config| is set audio stream will be configured
  absl::optional<PeerConnectionE2EQualityTestFixture::AudioConfig> audio_config;
  // If |rtc_event_log_path| is set, an RTCEventLog will be saved in that
>>>>>>> parent of 8e32ad0e8387 (revert libwebrtc changes to help bump)
  // location and it will be available for further analysis.
  absl::optional<std::string> rtc_event_log_path;
  // If |aec_dump_path| is set, an AEC dump will be saved in that location and
  // it will be available for further analysis.
  absl::optional<std::string> aec_dump_path;

  bool use_ulp_fec = false;
  bool use_flex_fec = false;
  // Specifies how much video encoder target bitrate should be different than
  // target bitrate, provided by WebRTC stack. Must be greater then 0. Can be
  // used to emulate overshooting of video encoders. This multiplier will
  // be applied for all video encoder on both sides for all layers. Bitrate
  // estimated by WebRTC stack will be multiplied by this multiplier and then
  // provided into VideoEncoder::SetRates(...).
  double video_encoder_bitrate_multiplier = 1.0;

  PeerConnectionInterface::RTCConfiguration rtc_configuration;
  PeerConnectionInterface::RTCOfferAnswerOptions rtc_offer_answer_options;
  BitrateSettings bitrate_settings;
};

// Contains parameters that maybe changed by test writer during the test call.
struct ConfigurableParams {
  // If `video_configs` is empty - no video should be added to the test call.
  std::vector<PeerConnectionE2EQualityTestFixture::VideoConfig> video_configs;

  PeerConnectionE2EQualityTestFixture::VideoSubscription video_subscription =
      PeerConnectionE2EQualityTestFixture::VideoSubscription()
          .SubscribeToAllPeers();
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // TEST_PC_E2E_PEER_CONNECTION_QUALITY_TEST_PARAMS_H_
