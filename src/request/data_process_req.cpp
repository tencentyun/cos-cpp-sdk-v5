#include "request/data_process_req.h"

#include "cos_sys_config.h"
#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

namespace qcloud_cos {

void PutImageByFileReq:: CheckCoverOriginImage() {
  std::vector<PicRules> pic_rules = m_pic_operation.GetRules();
  std::string object_name = GetObjectName();
  std::string object_dir = object_name.substr(0, object_name.find_last_of("/") + 1);
  for (std::vector<PicRules>::const_iterator c_itr = pic_rules.begin();
       c_itr != pic_rules.end(); ++c_itr) {
    const PicRules& pic_rule = *c_itr;
    std::string pic_absolute_key;
    if (StringUtil::StringStartsWith(pic_rule.fileid, "/")) {
      // fileid 以“/”开头为绝对路径
      pic_absolute_key = StringUtil::StringRemovePrefix(pic_rule.fileid, "/");
    } else {
      // fileid 不以“/”开头为相对路径
      pic_absolute_key = object_dir + pic_rule.fileid;
    }
    // 如果图片处理的效果图存放路径与上传文件路径相同，则会覆盖上传的原文件
    // 覆盖上传原文件后Etag与本地文件不相同，此时不比较Etag
    if (pic_absolute_key == object_name) {
      TurnOffCheckETag();
      TurnOffComputeConentMd5();
      break;
    }
  }
}

bool CreateDocProcessJobsReq::GenerateRequestBody(std::string* body) const {
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  root_node->append_node(doc.allocate_node(rapidxml::node_element, "Tag",
                                           doc.allocate_string(m_tag.c_str())));

  rapidxml::xml_node<>* input_node =
      doc.allocate_node(rapidxml::node_element, "Input", NULL);
  input_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Object",
                        doc.allocate_string(m_input.object.c_str())));
  root_node->append_node(input_node);

  rapidxml::xml_node<>* operation_node =
      doc.allocate_node(rapidxml::node_element, "Operation", NULL);

  rapidxml::xml_node<>* operation_output_node =
      doc.allocate_node(rapidxml::node_element, "Output", NULL);
  operation_output_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Region",
      doc.allocate_string(m_operation.output.region.c_str())));
  operation_output_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Bucket",
      doc.allocate_string(m_operation.output.bucket.c_str())));
  operation_output_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Object",
      doc.allocate_string(m_operation.output.object.c_str())));
  operation_node->append_node(operation_output_node);

  rapidxml::xml_node<>* operation_doc_process_node =
      doc.allocate_node(rapidxml::node_element, "DocProcess", NULL);
  if (!m_operation.doc_process.src_type.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "SrcType",
        doc.allocate_string(m_operation.doc_process.src_type.c_str())));
  }
  if (!m_operation.doc_process.tgt_type.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "TgtType",
        doc.allocate_string(m_operation.doc_process.tgt_type.c_str())));
  }
  if (m_operation.doc_process.sheet_id > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "SheetId",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.sheet_id)
                .c_str())));
  }
  if (m_operation.doc_process.start_page > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "StartPage",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.start_page)
                .c_str())));
  }
  if (m_operation.doc_process.end_page > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "EndPage",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.end_page)
                .c_str())));
  }
  if (!m_operation.doc_process.image_params.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "ImageParams",
        doc.allocate_string(m_operation.doc_process.image_params.c_str())));
  }
  if (!m_operation.doc_process.doc_passwd.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "DocPassword",
        doc.allocate_string(m_operation.doc_process.doc_passwd.c_str())));
  }
  if (m_operation.doc_process.comments > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "Comments",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.comments)
                .c_str())));
  }
  if (m_operation.doc_process.paper_direction > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "PaperDirection",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.paper_direction)
                .c_str())));
  }
  if (m_operation.doc_process.quality > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "Quality",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.quality).c_str())));
  }
  if (m_operation.doc_process.zoom > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "Zoom",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.zoom).c_str())));
  }
  operation_node->append_node(operation_doc_process_node);

  root_node->append_node(operation_node);

  root_node->append_node(
      doc.allocate_node(rapidxml::node_element, "QueueId",
                        doc.allocate_string(m_queue_id.c_str())));

  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();

  return true;
}

bool UpdateQueueReq::GenerateRequestBody(std::string* body) const {
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  root_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Name", doc.allocate_string(m_name.c_str())));
  root_node->append_node(
      doc.allocate_node(rapidxml::node_element, "QueueID",
                        doc.allocate_string(m_queue_id.c_str())));
  root_node->append_node(doc.allocate_node(
      rapidxml::node_element, "State", doc.allocate_string(m_state.c_str())));

  rapidxml::xml_node<>* notify_config_node =
      doc.allocate_node(rapidxml::node_element, "NotifyConfig", NULL);
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Url",
                        doc.allocate_string(m_notify_config.url.c_str())));
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Type",
                        doc.allocate_string(m_notify_config.type.c_str())));
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Event",
                        doc.allocate_string(m_notify_config.event.c_str())));
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "State",
                        doc.allocate_string(m_notify_config.state.c_str())));

  root_node->append_node(notify_config_node);

  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();

  return true;
}

void CreateDataProcessJobsReq::GenerateVideoNode(rapidxml::xml_document<>& doc, const Video& video, rapidxml::xml_node<>* video_node) {
#define TAG_STRING_FIELD(NODE, KEY, FIELD) \
  if (!KEY.empty()) { \
    NODE->append_node(doc.allocate_node( \
      rapidxml::node_element, FIELD, \
      doc.allocate_string(KEY.c_str()))); \
  }
  TAG_STRING_FIELD(video_node, video.codec, "Codec");
  TAG_STRING_FIELD(video_node, video.width, "Width");
  TAG_STRING_FIELD(video_node, video.height, "Height");
  TAG_STRING_FIELD(video_node, video.fps, "Fps");
  TAG_STRING_FIELD(video_node, video.remove, "Remove");
  TAG_STRING_FIELD(video_node, video.profile, "Profile");
  TAG_STRING_FIELD(video_node, video.bit_rate, "Bitrate");
  TAG_STRING_FIELD(video_node, video.crf, "Crt");
  TAG_STRING_FIELD(video_node, video.gop, "Gop");
  TAG_STRING_FIELD(video_node, video.preset, "Preset");
  TAG_STRING_FIELD(video_node, video.buf_size, "Bufsize");
  TAG_STRING_FIELD(video_node, video.max_rate, "Maxrate");
  TAG_STRING_FIELD(video_node, video.pixfmt, "Pixfmt");
  TAG_STRING_FIELD(video_node, video.long_short_mode, "LongShortMode");
  TAG_STRING_FIELD(video_node, video.rotate, "Rotate");
  TAG_STRING_FIELD(video_node, video.roi, "Roo");
  TAG_STRING_FIELD(video_node, video.crop, "Crop");
  TAG_STRING_FIELD(video_node, video.interlaced, "Interlaced");
  TAG_STRING_FIELD(video_node, video.animate_time_interval_of_frame, "AnimateTimeIntervalOfFrame");
  TAG_STRING_FIELD(video_node, video.animate_frames_per_second, "AnimateFramesPerSecond");
  TAG_STRING_FIELD(video_node, video.quality, "Quality");  
}

void CreateDataProcessJobsReq::GenerateAudioNode(rapidxml::xml_document<>& doc, const Audio& audio, rapidxml::xml_node<>* audio_node) {
#define TAG_STRING_FIELD(NODE, KEY, FIELD) \
  if (!KEY.empty()) { \
    NODE->append_node(doc.allocate_node( \
      rapidxml::node_element, FIELD, \
      doc.allocate_string(KEY.c_str()))); \
  }
  TAG_STRING_FIELD(audio_node, audio.codec, "Codec");
  TAG_STRING_FIELD(audio_node, audio.sample_rate, "Samplerate");
  TAG_STRING_FIELD(audio_node, audio.bit_rate, "Bitrate");
  TAG_STRING_FIELD(audio_node, audio.channels, "Channels");
  TAG_STRING_FIELD(audio_node, audio.remove, "Remove");
  TAG_STRING_FIELD(audio_node, audio.sample_format, "SampleFormat");
}

void CreateDataProcessJobsReq::GenerateContainerNode(rapidxml::xml_document<>& doc, const Container& container, rapidxml::xml_node<>* node) {
#define TAG_STRING_FIELD(NODE, KEY, FIELD) \
  if (!KEY.empty()) { \
    NODE->append_node(doc.allocate_node( \
      rapidxml::node_element, FIELD, \
      doc.allocate_string(KEY.c_str()))); \
  }
  TAG_STRING_FIELD(node, container.format, "Format");  
  // clip_config
  {
    rapidxml::xml_node<>* container_clip_config_node =
      doc.allocate_node(rapidxml::node_element, "ClipConfig", NULL);
    TAG_STRING_FIELD(container_clip_config_node, container.clip_config.duration, "Duration");   
    node->append_node(container_clip_config_node);
  }
}

void CreateDataProcessJobsReq::GenerateAudioMixNode(rapidxml::xml_document<>& doc, const AudioMix& audio_mix, rapidxml::xml_node<>* node) {
#define TAG_STRING_FIELD(NODE, KEY, FIELD) \
  if (!KEY.empty()) { \
    NODE->append_node(doc.allocate_node( \
      rapidxml::node_element, FIELD, \
      doc.allocate_string(KEY.c_str()))); \
  }
  TAG_STRING_FIELD(node, audio_mix.audio_source, "AudioSource");   
  TAG_STRING_FIELD(node, audio_mix.mix_mode, "MixMode");   
  TAG_STRING_FIELD(node, audio_mix.replace, "Replace");   
  // effect_config
  {
    rapidxml::xml_node<>* effect_config_node =
      doc.allocate_node(rapidxml::node_element, "EffectConfig", NULL);
    TAG_STRING_FIELD(effect_config_node, audio_mix.effect_config.enable_start_fade_in, "EnableStartFadein");   
    TAG_STRING_FIELD(effect_config_node, audio_mix.effect_config.start_fade_in_time, "StartFadeinTime");   
    TAG_STRING_FIELD(effect_config_node, audio_mix.effect_config.enable_end_fade_out, "EnableEndFadeout");   
    TAG_STRING_FIELD(effect_config_node, audio_mix.effect_config.end_fade_out_time, "EndFadeoutTime");   
    TAG_STRING_FIELD(effect_config_node, audio_mix.effect_config.enable_bgm_fade, "EnableBgmFade");   
    TAG_STRING_FIELD(effect_config_node, audio_mix.effect_config.bgm_fade_time, "BgmFadeTime");   
    node->append_node(effect_config_node);        
  }
}

bool CreateDataProcessJobsReq::GenerateRequestBody(std::string* body) const {
#define TAG_STRING_FIELD(NODE, KEY, FIELD) \
  if (!KEY.empty()) { \
    NODE->append_node(doc.allocate_node( \
      rapidxml::node_element, FIELD, \
      doc.allocate_string(KEY.c_str()))); \
  }

 rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  // tag
  root_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Tag", doc.allocate_string(
        options_.tag.c_str())));

  // queueId
  if (!options_.queue_id.empty()) {
    root_node->append_node(
      doc.allocate_node(rapidxml::node_element, "QueueId",
                        doc.allocate_string(options_.queue_id.c_str())));
  }      

  // input
  {
    rapidxml::xml_node<>* input_node =
        doc.allocate_node(rapidxml::node_element, "Input", NULL);
    if (!options_.input.object.empty()) {
      input_node->append_node(
          doc.allocate_node(rapidxml::node_element, "Object",
                          doc.allocate_string(options_.input.object.c_str())));
    }
    if (!options_.input.url.empty()) { 
     input_node->append_node(
          doc.allocate_node(rapidxml::node_element, "Url",
                          doc.allocate_string(options_.input.url.c_str())));
    }
    root_node->append_node(input_node);
  }
  
  rapidxml::xml_node<>* operation_node =
      doc.allocate_node(rapidxml::node_element, "Operation", NULL);

  // output
  {
    rapidxml::xml_node<>* operation_output_node =
        doc.allocate_node(rapidxml::node_element, "Output", NULL);
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.region, "Region");
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.bucket, "Bucket");
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.object, "Object");
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.sprite_object, "SpriteObject");
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.au_object, "AuObject");
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.bass_object, "BassObject");
    TAG_STRING_FIELD(operation_output_node, options_.operation.output.drum_object, "DrumObject");
    operation_node->append_node(operation_output_node);
  }

  // job_level
  if (options_.operation.job_level != 0) {
    operation_node->append_node(doc.allocate_node(
      rapidxml::node_element, "JobLevel",
      doc.allocate_string(std::to_string(options_.operation.job_level).c_str())));
  }

  // user_data
  TAG_STRING_FIELD(operation_node, options_.operation.user_data, "UserData");

  // templateId
  TAG_STRING_FIELD(operation_node, options_.operation.template_id, "TemplateId");


  // snapshot
  {
    rapidxml::xml_node<>* operation_snapshot_node =
        doc.allocate_node(rapidxml::node_element, "Snapshot", NULL);
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.mode, "Mode");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.start, "Start");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.time_interval, "TimeInterval");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.count, "Count");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.width, "Width");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.height, "Height");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.ci_param, "CIParam");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.is_check_count, "IsCheckCount");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.is_check_black, "IsCheckBlack");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.black_level, "BlackLevel");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.pixel_black_threshold, "PixelBlackThreshold");
    TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.snap_shot_out_mode, "SnapshotOutMode");

    rapidxml::xml_node<>* operation_snapshot_sprite_snapshot_config_node =
        doc.allocate_node(rapidxml::node_element, "SpriteSnapshotConfig", NULL);
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.cell_width, "CellWidth");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.cell_height, "CellHeight");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.margin, "Margin");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.padding, "Padding");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.color, "Color");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.columns, "Columns");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.lines, "Lines");
    TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.scale_method, "ScaleMethod");
    operation_snapshot_node->append_node(operation_snapshot_sprite_snapshot_config_node);
    operation_node->append_node(operation_snapshot_node);
  }

  // transcode
  {
    rapidxml::xml_node<>* operation_transcode_node =
      doc.allocate_node(rapidxml::node_element, "Transcode", NULL);
    // time_interval
    {
      rapidxml::xml_node<>* transcode_time_interval_node =
        doc.allocate_node(rapidxml::node_element, "TimeInterval", NULL);
      TAG_STRING_FIELD(transcode_time_interval_node, options_.operation.transcode.time_interval.duration, "Duration");  
      TAG_STRING_FIELD(transcode_time_interval_node, options_.operation.transcode.time_interval.start, "Start");  
      operation_transcode_node->append_node(transcode_time_interval_node);
    }
    // container
    {
      rapidxml::xml_node<>* transcode_container_node =
        doc.allocate_node(rapidxml::node_element, "Container", NULL);
      GenerateContainerNode(doc, options_.operation.transcode.container, transcode_container_node);        
      operation_transcode_node->append_node(transcode_container_node);
    }
    // video
    {
      rapidxml::xml_node<>* transcode_video_node =
        doc.allocate_node(rapidxml::node_element, "Video", NULL);
      GenerateVideoNode(doc, options_.operation.transcode.video, transcode_video_node);
      operation_transcode_node->append_node(transcode_video_node);
    }
    // audio
    {
      rapidxml::xml_node<>* transcode_audio_node =
        doc.allocate_node(rapidxml::node_element, "Audio", NULL);
      GenerateAudioNode(doc, options_.operation.transcode.audio, transcode_audio_node);
      operation_transcode_node->append_node(transcode_audio_node);
    }
    // trans_config
    {
      rapidxml::xml_node<>* transcode_trans_config_node =
        doc.allocate_node(rapidxml::node_element, "TransConfig", NULL);
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.adj_dar_method, "AdjDarMethod");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.is_check_reso, "IsCheckReso");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.reso_adj_method, "ResoAdjMethod");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.is_check_video_bit_rate, "IsCheckVideoBitrate");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.video_bit_rate_adj_method, "VideoBitrateAdjMethod");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.is_check_audio_bit_rate, "IsCheckAudioBitrate");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.audio_bit_rate_adj_method, "AudioBitrateAdjMethod");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.is_check_video_fps, "IsCheckVideoFps");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.video_fps_adj_method, "VideoFpsAdjMethod");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.delete_meta_data, "DeleteMetadata");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.is_hdr_2_sdr, "IsHdr2Sdr");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.transcode_index, "TranscodeIndex");
      TAG_STRING_FIELD(transcode_trans_config_node, options_.operation.transcode.trans_config.is_hdr_2_sdr, "Codec");
      // hls_encrypt
      {
        rapidxml::xml_node<>* trans_config_hls_encrypt_node =
          doc.allocate_node(rapidxml::node_element, "HlsEncrypt", NULL);
        TAG_STRING_FIELD(trans_config_hls_encrypt_node, options_.operation.transcode.trans_config.hls_encrypt.is_hls_encrypt, "IsHlsEncrypt");   
        TAG_STRING_FIELD(trans_config_hls_encrypt_node, options_.operation.transcode.trans_config.hls_encrypt.url_key, "UriKey");   
        transcode_trans_config_node->append_node(trans_config_hls_encrypt_node);
      }
      // dash_encrypt
      {
        rapidxml::xml_node<>* trans_config_dash_encrypt_node =
          doc.allocate_node(rapidxml::node_element, "DashEncrypt", NULL);
        TAG_STRING_FIELD(trans_config_dash_encrypt_node, options_.operation.transcode.trans_config.dash_encrypt.is_encrypt, "IsEncrypt");   
        TAG_STRING_FIELD(trans_config_dash_encrypt_node, options_.operation.transcode.trans_config.dash_encrypt.url_key, "UriKey");   
        transcode_trans_config_node->append_node(trans_config_dash_encrypt_node);
      }
      operation_transcode_node->append_node(transcode_trans_config_node);
    }    
    // audio_mix
    {
      rapidxml::xml_node<>* trans_audio_mix_node =
        doc.allocate_node(rapidxml::node_element, "AudioMix", NULL);
      GenerateAudioMixNode(doc, options_.operation.transcode.audio_mix, trans_audio_mix_node);
      operation_transcode_node->append_node(trans_audio_mix_node);
    }
    // audio_mix_array
    {
      for (auto iter = options_.operation.transcode.audio_mix_array.begin(); iter != options_.operation.transcode.audio_mix_array.end(); iter++) {
        rapidxml::xml_node<>* trans_audio_array_mix_node =
          doc.allocate_node(rapidxml::node_element, "AudioMixArray", NULL);
        GenerateAudioMixNode(doc, (*iter), trans_audio_array_mix_node);
        operation_transcode_node->append_node(trans_audio_array_mix_node);
      }
    }
    operation_node->append_node(operation_transcode_node);
  }

  // watermark_template_id
  for (auto iter = options_.operation.watermark_template_id.begin(); iter != options_.operation.watermark_template_id.end(); iter++) {
    TAG_STRING_FIELD(operation_node, (*iter), "WatermarkTemplateId");   
  }

  // watermark
  {
    for (auto iter = options_.operation.watermarks.begin(); iter != options_.operation.watermarks.end(); iter++) {
      rapidxml::xml_node<>* opertaion_watermark_node =
        doc.allocate_node(rapidxml::node_element, "Watermark", NULL);
      TAG_STRING_FIELD(opertaion_watermark_node, (*iter).type, "Type");
      TAG_STRING_FIELD(opertaion_watermark_node, (*iter).loc_mode, "LocMode");
      TAG_STRING_FIELD(opertaion_watermark_node, (*iter).dx, "Dx");
      TAG_STRING_FIELD(opertaion_watermark_node, (*iter).dy, "Dy");
      TAG_STRING_FIELD(opertaion_watermark_node, (*iter).start_time, "StartTime");
      TAG_STRING_FIELD(opertaion_watermark_node, (*iter).end_time, "EndTime");
      // slide_config
      {
        rapidxml::xml_node<>* watermark_slide_config_node =
          doc.allocate_node(rapidxml::node_element, "SlideConfig", NULL);
        TAG_STRING_FIELD(watermark_slide_config_node, (*iter).slide_config.slide_mode, "SlideMode");
        TAG_STRING_FIELD(watermark_slide_config_node, (*iter).slide_config.x_slide_speed, "XSlideSpeed");
        TAG_STRING_FIELD(watermark_slide_config_node, (*iter).slide_config.y_slide_speed, "YSlideSpeed");
        opertaion_watermark_node->append_node(watermark_slide_config_node);        
      }
      // Text
      {
        rapidxml::xml_node<>* watermark_text_node =
          doc.allocate_node(rapidxml::node_element, "Text", NULL);
        TAG_STRING_FIELD(watermark_text_node, (*iter).text.font_size, "FontSize");
        TAG_STRING_FIELD(watermark_text_node, (*iter).text.font_type, "FontType");
        TAG_STRING_FIELD(watermark_text_node, (*iter).text.font_color, "FontColor");
        TAG_STRING_FIELD(watermark_text_node, (*iter).text.transparency, "Transparency");
        TAG_STRING_FIELD(watermark_text_node, (*iter).text.text, "Text");
        opertaion_watermark_node->append_node(watermark_text_node);        
      }
      // Image
      {
        rapidxml::xml_node<>* watermark_image_node =
          doc.allocate_node(rapidxml::node_element, "Image", NULL);
        TAG_STRING_FIELD(watermark_image_node, (*iter).image.url, "Url");
        TAG_STRING_FIELD(watermark_image_node, (*iter).image.mode, "Mode");
        TAG_STRING_FIELD(watermark_image_node, (*iter).image.width, "Width");
        TAG_STRING_FIELD(watermark_image_node, (*iter).image.height, "Height");
        TAG_STRING_FIELD(watermark_image_node, (*iter).image.transparency, "Transparency");
        TAG_STRING_FIELD(watermark_image_node, (*iter).image.backgroud, "Background");
        opertaion_watermark_node->append_node(watermark_image_node);        
      }
      operation_node->append_node(opertaion_watermark_node);
    }
  }

  // animation
  {
    rapidxml::xml_node<>* operation_animation_node =
      doc.allocate_node(rapidxml::node_element, "Animation", NULL);
    // time_interval
    {
      rapidxml::xml_node<>* animation_time_interval_node =
        doc.allocate_node(rapidxml::node_element, "TimeInterval", NULL);
      TAG_STRING_FIELD(animation_time_interval_node, options_.operation.animation.time_interval.duration, "Duration");  
      TAG_STRING_FIELD(animation_time_interval_node, options_.operation.animation.time_interval.start, "Start");  
      operation_animation_node->append_node(animation_time_interval_node);
    }
    // container
    {
      rapidxml::xml_node<>* animation_container_node =
        doc.allocate_node(rapidxml::node_element, "Container", NULL);
      GenerateContainerNode(doc, options_.operation.animation.container, animation_container_node);  
      operation_animation_node->append_node(animation_container_node);
    }
    // video
    {
      rapidxml::xml_node<>* animation_video_node =
        doc.allocate_node(rapidxml::node_element, "Video", NULL);
      GenerateVideoNode(doc, options_.operation.animation.video, animation_video_node);
      operation_animation_node->append_node(animation_video_node);
    }    
  }

  // smart_cover 
  {
    rapidxml::xml_node<>* operation_smart_cover_node =
      doc.allocate_node(rapidxml::node_element, "SmartCover", NULL);
    TAG_STRING_FIELD(operation_smart_cover_node, options_.operation.smart_cover.format, "Format");
    TAG_STRING_FIELD(operation_smart_cover_node, options_.operation.smart_cover.width, "Width");
    TAG_STRING_FIELD(operation_smart_cover_node, options_.operation.smart_cover.height, "Height");
    TAG_STRING_FIELD(operation_smart_cover_node, options_.operation.smart_cover.count, "Count");
    TAG_STRING_FIELD(operation_smart_cover_node, options_.operation.smart_cover.delete_duplicates, "DeleteDuplicates");
    operation_node->append_node(operation_smart_cover_node);

  }

  // remove_watermark
  {
    rapidxml::xml_node<>* remove_watermark_node =
      doc.allocate_node(rapidxml::node_element, "RemoveWatermark", NULL);
    TAG_STRING_FIELD(remove_watermark_node, options_.operation.remove_watermark.dx, "Dx");
    TAG_STRING_FIELD(remove_watermark_node, options_.operation.remove_watermark.dy, "Dy");
    TAG_STRING_FIELD(remove_watermark_node, options_.operation.remove_watermark.width, "Width");
    TAG_STRING_FIELD(remove_watermark_node, options_.operation.remove_watermark.height, "Height");
    operation_node->append_node(remove_watermark_node); 
  }
  
  // segment
  {
    rapidxml::xml_node<>* segment_node =
      doc.allocate_node(rapidxml::node_element, "Segment", NULL);
    TAG_STRING_FIELD(segment_node, options_.operation.segment.format, "Format");
    TAG_STRING_FIELD(segment_node, options_.operation.segment.duration, "Duration");
    TAG_STRING_FIELD(segment_node, options_.operation.segment.transcode_index, "TranscodeIndex");
    TAG_STRING_FIELD(segment_node, options_.operation.segment.start_time, "StartTime");
    TAG_STRING_FIELD(segment_node, options_.operation.segment.end_time, "EndTime");
    rapidxml::xml_node<>* hls_encrypt_node =
          doc.allocate_node(rapidxml::node_element, "HlsEncrypt", NULL);
    TAG_STRING_FIELD(hls_encrypt_node, options_.operation.segment.hls_encrypt.is_hls_encrypt, "IsHlsEncrypt");   
    TAG_STRING_FIELD(hls_encrypt_node, options_.operation.segment.hls_encrypt.url_key, "UriKey");   
    segment_node->append_node(hls_encrypt_node);
    operation_node->append_node(segment_node); 
  }

  // subtitles
  {
    rapidxml::xml_node<>* subtitles_node =
      doc.allocate_node(rapidxml::node_element, "Subtitles", NULL);
    for (auto iter = options_.operation.subtitles.subtitle.begin(); iter != options_.operation.subtitles.subtitle.end(); iter++) {
      rapidxml::xml_node<>* subtitle_node =
        doc.allocate_node(rapidxml::node_element, "Subtitle", NULL);
      TAG_STRING_FIELD(subtitle_node, (*iter).url, "Url");
      subtitles_node->append_node(subtitle_node);
    }
    operation_node->append_node(subtitles_node); 
  }

  // digital_watermark
  {
    rapidxml::xml_node<>* digital_watermark_node =
      doc.allocate_node(rapidxml::node_element, "DigitalWatermark", NULL);
    TAG_STRING_FIELD(digital_watermark_node, options_.operation.digital_watermark.message, "Message");
    TAG_STRING_FIELD(digital_watermark_node, options_.operation.digital_watermark.type, "Type");
    TAG_STRING_FIELD(digital_watermark_node, options_.operation.digital_watermark.version, "version");
    TAG_STRING_FIELD(digital_watermark_node, options_.operation.digital_watermark.ignore_error, "ignore_error");
    TAG_STRING_FIELD(digital_watermark_node, options_.operation.digital_watermark.state, "state");
    operation_node->append_node(digital_watermark_node); 
  }

  // extract_digital_watermark
  {
    rapidxml::xml_node<>* extract_digital_watermark_node =
      doc.allocate_node(rapidxml::node_element, "ExtractDigitalWatermark", NULL);
    TAG_STRING_FIELD(extract_digital_watermark_node, options_.operation.extract_digital_watermark.type, "Type");
    TAG_STRING_FIELD(extract_digital_watermark_node, options_.operation.extract_digital_watermark.version, "version");
    operation_node->append_node(extract_digital_watermark_node); 
  }

  // file_uncompress
  {
    rapidxml::xml_node<>* operation_file_uncompress_config_node =
        doc.allocate_node(rapidxml::node_element, "FileUncompressConfig", NULL);
    TAG_STRING_FIELD(operation_file_uncompress_config_node, options_.operation.file_uncompress_config.prefix, "Prefix");
    TAG_STRING_FIELD(operation_file_uncompress_config_node, options_.operation.file_uncompress_config.un_compress_key, "UnCompressKey");
    TAG_STRING_FIELD(operation_file_uncompress_config_node, options_.operation.file_uncompress_config.prefix_replaced, "PrefixReplaced");
    operation_node->append_node(operation_file_uncompress_config_node);
  }
  root_node->append_node(operation_node);

  // voice_separate
  {
    rapidxml::xml_node<>* operation_voice_separate_node =
        doc.allocate_node(rapidxml::node_element, "VoiceSeparate", NULL);
    TAG_STRING_FIELD(operation_voice_separate_node, options_.operation.voice_separate.audio_mode, "AudioMode");
    rapidxml::xml_node<>* audio_config_node =
        doc.allocate_node(rapidxml::node_element, "AudioConfig", NULL);    
    TAG_STRING_FIELD(audio_config_node, options_.operation.voice_separate.audio_config.codec, "Codec");
    TAG_STRING_FIELD(audio_config_node, options_.operation.voice_separate.audio_config.channels, "Channels");
    TAG_STRING_FIELD(audio_config_node, options_.operation.voice_separate.audio_config.bit_rate, "Bitrate");
    TAG_STRING_FIELD(audio_config_node, options_.operation.voice_separate.audio_config.sample_rate, "Samplerate");
    operation_voice_separate_node->append_node(audio_config_node);
    operation_node->append_node(operation_voice_separate_node);    
  }

  // video_motage
  {
    rapidxml::xml_node<>* video_motage_node =
        doc.allocate_node(rapidxml::node_element, "VideoMotage", NULL);    
    TAG_STRING_FIELD(video_motage_node, options_.operation.video_montage.scene, "Scene");
    // container
    {
      rapidxml::xml_node<>* container_node =
        doc.allocate_node(rapidxml::node_element, "Container", NULL);
      GenerateContainerNode(doc, options_.operation.video_montage.container, container_node);  
      video_motage_node->append_node(container_node);
    }
    // video
    {
      rapidxml::xml_node<>* video_node =
        doc.allocate_node(rapidxml::node_element, "Video", NULL);
      GenerateVideoNode(doc, options_.operation.video_montage.video, video_node);
      video_motage_node->append_node(video_node);
    }
    // audio
    {
      rapidxml::xml_node<>* audio_node =
        doc.allocate_node(rapidxml::node_element, "Audio", NULL);
      GenerateAudioNode(doc, options_.operation.video_montage.audio, audio_node);
      video_motage_node->append_node(audio_node);
    }    
    // audio_mix
    {
      rapidxml::xml_node<>* audio_mix_node =
        doc.allocate_node(rapidxml::node_element, "AudioMix", NULL);
      GenerateAudioMixNode(doc, options_.operation.video_montage.audio_mix, audio_mix_node);
      video_motage_node->append_node(audio_mix_node);
    }
    // audio_mix_array
    {
      for (auto iter = options_.operation.concat.audio_mix_array.begin(); iter != options_.operation.concat.audio_mix_array.end(); iter++) {
        rapidxml::xml_node<>* audio_array_mix_node =
          doc.allocate_node(rapidxml::node_element, "AudioMixArray", NULL);
        GenerateAudioMixNode(doc, (*iter), audio_array_mix_node);
        video_motage_node->append_node(audio_array_mix_node);
      }
    }  
    operation_node->append_node(video_motage_node);    
  }

  // concat
  {
    rapidxml::xml_node<>* concat_node =
        doc.allocate_node(rapidxml::node_element, "ConcatTemplate", NULL);
    TAG_STRING_FIELD(concat_node, options_.operation.concat.index, "Index");
    TAG_STRING_FIELD(concat_node, options_.operation.concat.direct_concat, "DirectConcat");
    // container
    {
      rapidxml::xml_node<>* conccat_container_node =
        doc.allocate_node(rapidxml::node_element, "Container", NULL);
      GenerateContainerNode(doc, options_.operation.concat.container, conccat_container_node);  
      concat_node->append_node(conccat_container_node);
    }
    // video
    {
      rapidxml::xml_node<>* video_node =
        doc.allocate_node(rapidxml::node_element, "Video", NULL);
      GenerateVideoNode(doc, options_.operation.concat.video, video_node);
      concat_node->append_node(video_node);
    }
    // audio
    {
      rapidxml::xml_node<>* audio_node =
        doc.allocate_node(rapidxml::node_element, "Audio", NULL);
      GenerateAudioNode(doc, options_.operation.concat.audio, audio_node);
      concat_node->append_node(audio_node);
    }    
    // audio_mix
    {
      rapidxml::xml_node<>* audio_mix_node =
        doc.allocate_node(rapidxml::node_element, "AudioMix", NULL);
      GenerateAudioMixNode(doc, options_.operation.concat.audio_mix, audio_mix_node);        
      concat_node->append_node(audio_mix_node);
    }
    // audio_mix_array
    {
      for (auto iter = options_.operation.concat.audio_mix_array.begin(); iter != options_.operation.concat.audio_mix_array.end(); iter++) {
        rapidxml::xml_node<>* audio_array_mix_node =
          doc.allocate_node(rapidxml::node_element, "AudioMixArray", NULL);
        GenerateAudioMixNode(doc, (*iter), audio_array_mix_node);
        concat_node->append_node(audio_array_mix_node);
      }
    }    

    // concat_fragment
    {
      for (auto iter = options_.operation.concat.concat_fragment.begin(); iter != options_.operation.concat.concat_fragment.end(); iter++) {
        rapidxml::xml_node<>* concat_fragment_node =
          doc.allocate_node(rapidxml::node_element, "ConcatFragment", NULL);
        TAG_STRING_FIELD(concat_fragment_node, (*iter).url, "Url");  
        TAG_STRING_FIELD(concat_fragment_node, (*iter).fragment_index, "FragmentIndex");   
        TAG_STRING_FIELD(concat_fragment_node, (*iter).start_time, "StartTime");   
        TAG_STRING_FIELD(concat_fragment_node, (*iter).url, "EndTime");   
        concat_node->append_node(concat_fragment_node);
      }      
    }
    
    // scene_change_info
    {
      rapidxml::xml_node<>* scene_change_info_node =
        doc.allocate_node(rapidxml::node_element, "SceneChangeInfo", NULL);
      TAG_STRING_FIELD(scene_change_info_node, options_.operation.concat.scene_change_info.mode, "Mode");   
      TAG_STRING_FIELD(scene_change_info_node, options_.operation.concat.scene_change_info.time, "Time");   
      concat_node->append_node(scene_change_info_node);
    }
    operation_node->append_node(concat_node);    
  }

  // Callback
  TAG_STRING_FIELD(root_node, options_.callback_format, "CallBackFormat");
  TAG_STRING_FIELD(root_node, options_.callback_type, "CallBackType");
  TAG_STRING_FIELD(root_node, options_.callback, "CallBack");
  rapidxml::xml_node<>* callback_mq_config =
    doc.allocate_node(rapidxml::node_element, "CallBackMqConfig", NULL);
  TAG_STRING_FIELD(callback_mq_config, options_.callback_mq_config.mq_mode, "MqMode");
  TAG_STRING_FIELD(callback_mq_config, options_.callback_mq_config.mq_region, "MqRegion");
  TAG_STRING_FIELD(callback_mq_config, options_.callback_mq_config.mq_name, "MqName");
  root_node->append_node(callback_mq_config);

  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();
  return true;
}



}  // namespace qcloud_cos
