/*
 * ngtcp2
 *
 * Copyright (c) 2023 ngtcp2 contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "ngtcp2_conversion_test.h"

#include <CUnit/CUnit.h>

#include "ngtcp2_conversion.h"
#include "ngtcp2_test_helper.h"
#include "ngtcp2_net.h"

void test_ngtcp2_transport_params_convert_to_latest(void) {
  ngtcp2_transport_params *src, srcbuf, paramsbuf;
  const ngtcp2_transport_params *dest;
  size_t v1len;
  ngtcp2_cid rcid, scid, dcid;
  uint8_t available_versions[sizeof(uint32_t) * 3];
  ngtcp2_sockaddr_in6 *sa_in6;

  rcid_init(&rcid);
  scid_init(&scid);
  dcid_init(&dcid);

  ngtcp2_transport_params_default_versioned(NGTCP2_TRANSPORT_PARAMS_V1,
                                            &srcbuf);

  srcbuf.initial_max_stream_data_bidi_local = 1000000007;
  srcbuf.initial_max_stream_data_bidi_remote = 961748941;
  srcbuf.initial_max_stream_data_uni = 982451653;
  srcbuf.initial_max_data = 1000000009;
  srcbuf.initial_max_streams_bidi = 908;
  srcbuf.initial_max_streams_uni = 16383;
  srcbuf.max_idle_timeout = 16363 * NGTCP2_MILLISECONDS;
  srcbuf.max_udp_payload_size = 1200;
  srcbuf.stateless_reset_token_present = 1;
  memset(srcbuf.stateless_reset_token, 0xf1,
         sizeof(srcbuf.stateless_reset_token));
  srcbuf.ack_delay_exponent = 20;
  srcbuf.preferred_address_present = 1;
  srcbuf.preferred_address.ipv4_present = 0;
  sa_in6 = &srcbuf.preferred_address.ipv6;
  sa_in6->sin6_family = AF_INET6;
  memset(&sa_in6->sin6_addr, 0xe1, sizeof(sa_in6->sin6_addr));
  sa_in6->sin6_port = ngtcp2_htons(63111);
  srcbuf.preferred_address.ipv6_present = 1;
  scid_init(&srcbuf.preferred_address.cid);
  memset(srcbuf.preferred_address.stateless_reset_token, 0xd1,
         sizeof(srcbuf.preferred_address.stateless_reset_token));
  srcbuf.disable_active_migration = 1;
  srcbuf.max_ack_delay = 63 * NGTCP2_MILLISECONDS;
  srcbuf.retry_scid_present = 1;
  srcbuf.retry_scid = rcid;
  srcbuf.original_dcid = dcid;
  srcbuf.initial_scid = scid;
  srcbuf.active_connection_id_limit = 1073741824;
  srcbuf.max_datagram_frame_size = 63;
  srcbuf.grease_quic_bit = 1;
  srcbuf.version_info.chosen_version = NGTCP2_PROTO_VER_V1;
  srcbuf.version_info.available_versions = available_versions;
  srcbuf.version_info.available_versionslen =
      ngtcp2_arraylen(available_versions);
  srcbuf.version_info_present = 1;

  v1len = offsetof(ngtcp2_transport_params, placeholder_field1);

  src = malloc(v1len);

  memcpy(src, &srcbuf, v1len);

  dest = ngtcp2_transport_params_convert_to_latest(
      &paramsbuf, NGTCP2_TRANSPORT_PARAMS_V1, src);

  free(src);

  CU_ASSERT(dest == &paramsbuf);
  CU_ASSERT(srcbuf.initial_max_stream_data_bidi_local ==
            dest->initial_max_stream_data_bidi_local);
  CU_ASSERT(srcbuf.initial_max_stream_data_bidi_remote ==
            dest->initial_max_stream_data_bidi_remote);
  CU_ASSERT(srcbuf.initial_max_stream_data_uni ==
            dest->initial_max_stream_data_uni);
  CU_ASSERT(srcbuf.initial_max_data == dest->initial_max_data);
  CU_ASSERT(srcbuf.initial_max_streams_bidi == dest->initial_max_streams_bidi);
  CU_ASSERT(srcbuf.initial_max_streams_uni == dest->initial_max_streams_uni);
  CU_ASSERT(srcbuf.max_idle_timeout == dest->max_idle_timeout);
  CU_ASSERT(srcbuf.max_udp_payload_size == dest->max_udp_payload_size);
  CU_ASSERT(0 == memcmp(srcbuf.stateless_reset_token,
                        dest->stateless_reset_token,
                        sizeof(srcbuf.stateless_reset_token)));
  CU_ASSERT(srcbuf.ack_delay_exponent == dest->ack_delay_exponent);
  CU_ASSERT(srcbuf.preferred_address_present ==
            dest->preferred_address_present);
  CU_ASSERT(0 == memcmp(&srcbuf.preferred_address.ipv4,
                        &dest->preferred_address.ipv4,
                        sizeof(srcbuf.preferred_address.ipv4)));
  CU_ASSERT(srcbuf.preferred_address.ipv4_present ==
            dest->preferred_address.ipv4_present);
  CU_ASSERT(0 == memcmp(&srcbuf.preferred_address.ipv6,
                        &dest->preferred_address.ipv6,
                        sizeof(srcbuf.preferred_address.ipv6)));
  CU_ASSERT(srcbuf.preferred_address.ipv6_present ==
            dest->preferred_address.ipv6_present);
  CU_ASSERT(ngtcp2_cid_eq(&srcbuf.preferred_address.cid,
                          &dest->preferred_address.cid));
  CU_ASSERT(0 ==
            memcmp(srcbuf.preferred_address.stateless_reset_token,
                   dest->preferred_address.stateless_reset_token,
                   sizeof(srcbuf.preferred_address.stateless_reset_token)));
  CU_ASSERT(srcbuf.disable_active_migration == dest->disable_active_migration);
  CU_ASSERT(srcbuf.max_ack_delay == dest->max_ack_delay);
  CU_ASSERT(srcbuf.retry_scid_present == dest->retry_scid_present);
  CU_ASSERT(ngtcp2_cid_eq(&srcbuf.retry_scid, &dest->retry_scid));
  CU_ASSERT(ngtcp2_cid_eq(&srcbuf.initial_scid, &dest->initial_scid));
  CU_ASSERT(ngtcp2_cid_eq(&srcbuf.original_dcid, &dest->original_dcid));
  CU_ASSERT(srcbuf.active_connection_id_limit ==
            dest->active_connection_id_limit);
  CU_ASSERT(srcbuf.max_datagram_frame_size == dest->max_datagram_frame_size);
  CU_ASSERT(srcbuf.grease_quic_bit = dest->grease_quic_bit);
  CU_ASSERT(srcbuf.version_info_present == dest->version_info_present);
  CU_ASSERT(srcbuf.version_info.chosen_version ==
            dest->version_info.chosen_version);
  CU_ASSERT(0 == memcmp(srcbuf.version_info.available_versions,
                        dest->version_info.available_versions,
                        srcbuf.version_info.available_versionslen));
  CU_ASSERT(1000000007 == dest->placeholder_field1);
  CU_ASSERT(1000000009 == dest->placeholder_field2);
}

void test_ngtcp2_transport_params_convert_to_old(void) {
  ngtcp2_transport_params src, *dest, destbuf;
  size_t v1len;
  ngtcp2_cid rcid, scid, dcid;
  uint8_t available_versions[sizeof(uint32_t) * 3];
  ngtcp2_sockaddr_in6 *sa_in6;

  rcid_init(&rcid);
  scid_init(&scid);
  dcid_init(&dcid);

  v1len = offsetof(ngtcp2_transport_params, placeholder_field1);

  dest = malloc(v1len);

  ngtcp2_transport_params_default(&src);
  src.initial_max_stream_data_bidi_local = 1000000007;
  src.initial_max_stream_data_bidi_remote = 961748941;
  src.initial_max_stream_data_uni = 982451653;
  src.initial_max_data = 1000000009;
  src.initial_max_streams_bidi = 908;
  src.initial_max_streams_uni = 16383;
  src.max_idle_timeout = 16363 * NGTCP2_MILLISECONDS;
  src.max_udp_payload_size = 1200;
  src.stateless_reset_token_present = 1;
  memset(src.stateless_reset_token, 0xf1, sizeof(src.stateless_reset_token));
  src.ack_delay_exponent = 20;
  src.preferred_address_present = 1;
  src.preferred_address.ipv4_present = 0;
  sa_in6 = &src.preferred_address.ipv6;
  sa_in6->sin6_family = AF_INET6;
  memset(&sa_in6->sin6_addr, 0xe1, sizeof(sa_in6->sin6_addr));
  sa_in6->sin6_port = ngtcp2_htons(63111);
  src.preferred_address.ipv6_present = 1;
  scid_init(&src.preferred_address.cid);
  memset(src.preferred_address.stateless_reset_token, 0xd1,
         sizeof(src.preferred_address.stateless_reset_token));
  src.disable_active_migration = 1;
  src.max_ack_delay = 63 * NGTCP2_MILLISECONDS;
  src.retry_scid_present = 1;
  src.retry_scid = rcid;
  src.original_dcid = dcid;
  src.initial_scid = scid;
  src.active_connection_id_limit = 1073741824;
  src.max_datagram_frame_size = 63;
  src.grease_quic_bit = 1;
  src.version_info.chosen_version = NGTCP2_PROTO_VER_V1;
  src.version_info.available_versions = available_versions;
  src.version_info.available_versionslen = ngtcp2_arraylen(available_versions);
  src.version_info_present = 1;

  ngtcp2_transport_params_convert_to_old(NGTCP2_TRANSPORT_PARAMS_V1, dest,
                                         &src);

  memset(&destbuf, 0, sizeof(destbuf));
  memcpy(&destbuf, dest, v1len);

  free(dest);

  CU_ASSERT(src.initial_max_stream_data_bidi_local ==
            destbuf.initial_max_stream_data_bidi_local);
  CU_ASSERT(src.initial_max_stream_data_bidi_remote ==
            destbuf.initial_max_stream_data_bidi_remote);
  CU_ASSERT(src.initial_max_stream_data_uni ==
            destbuf.initial_max_stream_data_uni);
  CU_ASSERT(src.initial_max_data == destbuf.initial_max_data);
  CU_ASSERT(src.initial_max_streams_bidi == destbuf.initial_max_streams_bidi);
  CU_ASSERT(src.initial_max_streams_uni == destbuf.initial_max_streams_uni);
  CU_ASSERT(src.max_idle_timeout == destbuf.max_idle_timeout);
  CU_ASSERT(src.max_udp_payload_size == destbuf.max_udp_payload_size);
  CU_ASSERT(0 == memcmp(src.stateless_reset_token,
                        destbuf.stateless_reset_token,
                        sizeof(src.stateless_reset_token)));
  CU_ASSERT(src.ack_delay_exponent == destbuf.ack_delay_exponent);
  CU_ASSERT(src.preferred_address_present == destbuf.preferred_address_present);
  CU_ASSERT(0 == memcmp(&src.preferred_address.ipv4,
                        &destbuf.preferred_address.ipv4,
                        sizeof(src.preferred_address.ipv4)));
  CU_ASSERT(src.preferred_address.ipv4_present ==
            destbuf.preferred_address.ipv4_present);
  CU_ASSERT(0 == memcmp(&src.preferred_address.ipv6,
                        &destbuf.preferred_address.ipv6,
                        sizeof(src.preferred_address.ipv6)));
  CU_ASSERT(src.preferred_address.ipv6_present ==
            destbuf.preferred_address.ipv6_present);
  CU_ASSERT(ngtcp2_cid_eq(&src.preferred_address.cid,
                          &destbuf.preferred_address.cid));
  CU_ASSERT(0 == memcmp(src.preferred_address.stateless_reset_token,
                        destbuf.preferred_address.stateless_reset_token,
                        sizeof(src.preferred_address.stateless_reset_token)));
  CU_ASSERT(src.disable_active_migration == destbuf.disable_active_migration);
  CU_ASSERT(src.max_ack_delay == destbuf.max_ack_delay);
  CU_ASSERT(src.retry_scid_present == destbuf.retry_scid_present);
  CU_ASSERT(ngtcp2_cid_eq(&src.retry_scid, &destbuf.retry_scid));
  CU_ASSERT(ngtcp2_cid_eq(&src.initial_scid, &destbuf.initial_scid));
  CU_ASSERT(ngtcp2_cid_eq(&src.original_dcid, &destbuf.original_dcid));
  CU_ASSERT(src.active_connection_id_limit ==
            destbuf.active_connection_id_limit);
  CU_ASSERT(src.max_datagram_frame_size == destbuf.max_datagram_frame_size);
  CU_ASSERT(src.grease_quic_bit = destbuf.grease_quic_bit);
  CU_ASSERT(src.version_info_present == destbuf.version_info_present);
  CU_ASSERT(src.version_info.chosen_version ==
            destbuf.version_info.chosen_version);
  CU_ASSERT(0 == memcmp(src.version_info.available_versions,
                        destbuf.version_info.available_versions,
                        src.version_info.available_versionslen));
}
