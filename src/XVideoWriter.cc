/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               �������Ĳܿ����˴��������Ϊѧϰ�ο�                       **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : FFmpeg
** Description   : FFMPEG��Ŀ����ʾ��
** Contact       : xiacaojun@qq.com
**        ����   : http://blog.csdn.net/jiedichina
**		��Ƶ�γ� : http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961
**                 http://edu.51cto.com/lecturer/index/user_id-12016059.html
**                 http://study.163.com/u/xiacaojun
**                 https://jiedi.ke.qq.com/
**   FFmpeg����Ƶ����ʵս �γ�Ⱥ ��651163356
**   ΢�Ź��ں�  : jiedi2007
**		ͷ����	 : �Ĳܿ�
** Creation date : 2017-05-17
**
*******************************************************************************/
#include "XVideoWriter.h"
extern "C" {
#include "third_party/ffmpeg/libavcodec/avcodec.h"
#include "third_party/ffmpeg/libavformat/avformat.h"
#include "third_party/ffmpeg/libswscale/swscale.h"
#include "third_party/ffmpeg/libavutil/opt.h"
#include "third_party/ffmpeg/libswresample/swresample.h"
}
#include <iostream>
using namespace std;
// #pragma comment(lib,"avformat.lib")
// #pragma comment(lib,"avcodec.lib")
// #pragma comment(lib,"avutil.lib")
// #pragma comment(lib,"swscale.lib")
// #pragma comment(lib,"swresample.lib")
#pragma warning(disable:4996)

class CXVideoWriter :public XVideoWriter {
public:
	AVFormatContext *ic = NULL;	//��װmp4���������
	AVCodecContext *vc = NULL;	//��Ƶ������������
	AVCodecContext *ac = NULL;	//��Ƶ������������
	AVStream *vs = NULL;		//��Ƶ��
	AVStream *as = NULL;		//��Ƶ��
	SwsContext *vsc = NULL;		//����ת����������
	SwrContext *asc = NULL;		//��Ƶ�ز���������
	AVFrame *yuv = NULL;		//���yuv
	AVFrame *pcm = NULL;		//�ز����������PCM
	int vpts = 0;				//��Ƶ��pts
	int apts = 0;				//��Ƶ��pts

	void Close() {
		if (ic) avformat_close_input(&ic);

		if (vc) {
			avcodec_close(vc);
			avcodec_free_context(&vc);
		}
		if (ac) {
			avcodec_close(ac);
			avcodec_free_context(&ac);
		}
		
		if (vsc) {
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (yuv)av_frame_free(&yuv);
		if (pcm)av_frame_free(&pcm);
		if (asc) {
			swr_free(&asc);
		}
	}

	bool Init(const char* file) {
		Close();
		//��װ�ļ����������
		avformat_alloc_output_context2(&ic, NULL, NULL, file);
		if (!ic) {
			cerr << "avformat_alloc_output_context2 failed!" << endl;
			return false;
		}
		filename = file;
		return true;
	}

	bool AddVideoStream() {
		if (!ic) return false;

		//1 ��Ƶ����������
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec) {
			cerr << "avcodec_find_encoder AV_CODEC_ID_H264 failed!" << endl;
			return false;
		}
		vc = avcodec_alloc_context3(codec);
		if (!vc) {
			cerr << "avcodec_alloc_context3 failed!" << endl;
			return false;
		}
		//�����ʣ�ѹ����ÿ���С
		vc->bit_rate = vBitrate;
		vc->width = outWidth;
		vc->height = outHeight;

		//ʱ�����
		vc->time_base = { 1, outFPS };
		vc->framerate = { outFPS, 1 };

		//�������С������֡һ���ؼ�֡
		vc->gop_size = 50;

		vc->max_b_frames = 0;
		
		vc->pix_fmt = AV_PIX_FMT_YUV420P;
		vc->codec_id = AV_CODEC_ID_H264;
		av_opt_set(vc->priv_data, "preset", "superfast", 0);
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		//�򿪱�����
		int ret = avcodec_open2(vc, codec, NULL);
		if (ret != 0) {
			cerr << "avcodec_open2 failed!" << endl;
			return false;
		}
		cout << "avcodec_open2 success!" <<endl;

		//�����Ƶ�������������
		vs = avformat_new_stream(ic, NULL);
		vs->codecpar->codec_tag = 0;

		avcodec_parameters_from_context(vs->codecpar, vc);

		av_dump_format(ic, 0, filename.c_str(), 1);
		
		//���أ��ߴ磩ת�������� rgb to yuv
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, (AVPixelFormat)inPixFmt,
			outWidth, outHeight, AV_PIX_FMT_YUV420P,
			SWS_BICUBIC,
			NULL,NULL,NULL
			);
		if (!vsc) {
			cerr << "sws_getCachedContext failed!" << endl;
			return false;
		}

		if (!yuv) {
			yuv = av_frame_alloc();
			yuv->format = AV_PIX_FMT_YUV420P;
			yuv->width = outWidth;
			yuv->height = outHeight;
			yuv->pts = 0;
			int ret = av_frame_get_buffer(yuv, 32);
			if (ret != 0) {
				cerr << "av_frame_get_buffer failed!" << endl;
				return false;
			}
		}
		return true;
	}

	AVPacket * EncodeVideo(const unsigned char *rgb) {
		if (!ic || !vsc || !yuv)return NULL;
		AVPacket *p = NULL;
		uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (uint8_t *)rgb;

		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		insize[0] = inWidth * 4;


		//rgb to yuv
		int h = sws_scale(vsc, indata, insize, 0, inHeight,
			yuv->data, yuv->linesize
			);
		if (h < 0)
			return NULL;
		//cout << h << "|";
		yuv->pts = vpts;
		vpts++;
		//encode
		int ret = avcodec_send_frame(vc, yuv);
		if (ret != 0) {
			return NULL;
		}
		p = av_packet_alloc();
		ret = avcodec_receive_packet(vc, p);
		if (ret != 0 ||p->size<=0) {
			av_packet_free(&p);
			return NULL;
		}

		av_packet_rescale_ts(p, vc->time_base, vs->time_base);
		p->stream_index = vs->index;
		return p;
	}

	bool WriteHead() {
		if (!ic)return false;

		//��io
		int ret = avio_open(&ic->pb, filename.c_str(), AVIO_FLAG_WRITE);
		if (ret!= 0) {
			cerr << "avio_open failed!" << endl;
			return false;
		}
		//д���װͷ
		ret = avformat_write_header(ic, NULL);
		if (ret != 0) {
			cerr << "avformat_write_header failed!" << endl;
			return false;
		}
		cout << "write " << filename<<" head success" << endl;
		return true;
	}

	bool WriteFrame(AVPacket *pkt) {
		if (!ic || !pkt || pkt->size <= 0) return false;
		//av_write_frame
		if (av_interleaved_write_frame(ic, pkt) != 0)
			return false;
		return true;
	}

	virtual bool WriteEnd() {
		if (!ic || !ic->pb) return false;
		
		//д��β����Ϣ����
		if (av_write_trailer(ic) != 0) {
			cerr << "av_write_trailer failed!" << endl;
			return false;
		}

		//�ر�����io
		if (avio_closep(&ic->pb) != 0) {
			cerr << "avio_close failed!" << endl;
			return false;
		}
		cout << "WriteEnd success!" << endl;

		return true;
	}

	bool AddAudioStream() {
		if (!ic) return false;
		//�ҵ���Ƶ����
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if (!codec) {
			cerr << "avcodec_find_encoder  AV_CODEC_ID_AAC failed!" << endl;
			return false;
		}
		//1����������Ƶ������
		ac = avcodec_alloc_context3(codec);
		if (!ac) {
			cerr << "avcodec_alloc_context3 failed!" << endl;
			return false;
		}
		ac->bit_rate = aBitrate;
		ac->sample_rate = outSampleRate;
		ac->sample_fmt = (AVSampleFormat)outSampleFmt;
		ac->channels = outChannels;
		ac->channel_layout = av_get_default_channel_layout(outChannels);
		ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		int ret = avcodec_open2(ac, codec, NULL);
		if (ret != 0) {
			avcodec_free_context(&ac);
			cerr << "avcodec_open2 failed!" << endl;
			return false;
		}
		cout << "avcodec_open2 AV_CODEC_ID_AAC success " << endl;

		//2 ������Ƶ��
		as = avformat_new_stream(ic, NULL);
		if (!as) {
			cerr << "avformat_new_stream failed!" << endl;
			return false;
		}
		as->codecpar->codec_tag = 0;
		avcodec_parameters_from_context(as->codecpar, ac);

		av_dump_format(ic, 0, filename.c_str(), 1);

		//3 ��Ƶ�ز��������ļ�����
		asc = swr_alloc_set_opts(asc,
			ac->channel_layout,ac->sample_fmt,ac->sample_rate,	//�����ʽ
			av_get_default_channel_layout(inChannels), (AVSampleFormat)inSampleFmt,inSampleRate,
			0,0
			);
		ret = swr_init(asc);
		if (ret != 0) {
			cerr << "swr_init failed !" << endl;
			return false;
		}

		//4 ��Ƶ�ز��������AVFrame
		if (!pcm) {
			pcm = av_frame_alloc();
			pcm->format = ac->sample_fmt;
			pcm->channels = ac->channels;
			pcm->channel_layout = ac->channel_layout;
			pcm->nb_samples = nb_sample;//һ֡��Ƶ����������
			ret = av_frame_get_buffer(pcm, 0);
			if (ret < 0) {
				cerr << "av_frame_get_buffer failed! " << endl;
				return false;
			}
			cout << "audio AVFrame create success!" << endl;
		}
		return true;
	}

	AVPacket * EncodeAudio(const unsigned char *d) {
		if (!ic || !asc || !pcm)return NULL;
		//1 ��Ƶ�ز���
		const uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
		data[0] = (uint8_t *)d;
		int len  = swr_convert(asc, pcm->data, pcm->nb_samples,
			data, pcm->nb_samples
			);
		//cout << len << "*";

		//2��Ƶ�ı���
		int ret = avcodec_send_frame(ac, pcm);
		if (ret != 0) {
			return NULL;
		}
		AVPacket *pkt = av_packet_alloc();
		av_init_packet(pkt);
		ret = avcodec_receive_packet(ac, pkt);
		if (ret != 0) {
			av_packet_free(&pkt);
			return NULL;
		}
		cout << pkt->size << "|";
		pkt->stream_index = as->index;

		pkt->pts = apts;
		pkt->dts = pkt->pts;
		apts += av_rescale_q(pcm->nb_samples, { 1, ac->sample_rate }, ac->time_base);

		return pkt;
	}

	bool IsVideoBefor() {
		if (!ic || !as || !vs) return false;
		int re = av_compare_ts(vpts, vc->time_base, apts, ac->time_base);
		if (re <= 0)
			return true;
		return false;
	}
};

XVideoWriter * XVideoWriter::Get(unsigned short index) {
	static bool isfirst = true;
	if (isfirst) {
		av_register_all();
		avcodec_register_all();
		isfirst = false;
	}

	static CXVideoWriter wrs[65535];
	return &wrs[index];
}

XVideoWriter::XVideoWriter() {}


XVideoWriter::~XVideoWriter() {}
