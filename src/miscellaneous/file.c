#include "file.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

static uint32_t read_be32(FILE *f) {
    uint32_t v; fread(&v,4,1,f); return ntohl(v);
}

static uint32_t read_le32(FILE *f) {
    uint32_t v; fread(&v,4,1,f); return le32toh(v);
}

const char* get_file_format(const char* filename) {
    FILE *file = fopen(filename,"rb"); if(!file) return "Unknown";
    unsigned char buffer[64]; size_t n = fread(buffer,1,sizeof(buffer),file);
    fclose(file);
    if(n>=12){
        if(memcmp(buffer,"RIFF",4)==0) return "WAV";
        if(memcmp(buffer,"fLaC",4)==0) return "FLAC";
        if(memcmp(buffer,"OggS",4)==0) return "OGG";
        if(memcmp(buffer,"ID3",3)==0) return "MP3";
        if((buffer[0]==0xFF)&&((buffer[1]&0xE0)==0xE0)) return "MP3";
        if(memcmp(buffer,"MThd",4)==0) return "MIDI";
        if(memcmp(buffer,"OpusHead",8)==0) return "OPUS";
        if(memcmp(buffer+28,"OpusHead",8)==0) return "OPUS";
        if(memcmp(buffer,"\x30\x26\xB2\x75",4)==0) return "WMA";
        if(memcmp(buffer+4,"ftypM4A",7)==0||memcmp(buffer+4,"ftypisom",8)==0) return "M4A";
    }
    return "Unknown";
}

int get_metadata(const char* filename, FileInfo* info) {
    memset(info,0,sizeof(FileInfo));
    const char* type = get_file_format(filename);
    strncpy(info->format,type,sizeof(info->format)-1);

    FILE* f = fopen(filename,"rb"); if(!f) return -1;

    if(strcmp(type,"MP3")==0){
        unsigned char hdr[10]; fread(hdr,1,10,f);
        if(memcmp(hdr,"ID3",3)==0){
            int size = ((hdr[6]&0x7F)<<21)|((hdr[7]&0x7F)<<14)|((hdr[8]&0x7F)<<7)|(hdr[9]&0x7F);
            unsigned char* data = malloc(size); fread(data,1,size,f);
            int pos=0;
            while(pos+10<size){
                char fid[5]={0}; memcpy(fid,data+pos,4);
                int fsize=(data[pos+4]<<24)|(data[pos+5]<<16)|(data[pos+6]<<8)|data[pos+7];
                if(fsize<=0) break;
                if(strcmp(fid,"TIT2")==0) strncpy(info->title,(char*)data+pos+10,fsize);
                if(strcmp(fid,"TPE1")==0) strncpy(info->artist,(char*)data+pos+10,fsize);
                if(strcmp(fid,"TALB")==0) strncpy(info->album,(char*)data+pos+10,fsize);
                if(strcmp(fid,"TCON")==0) strncpy(info->genre,(char*)data+pos+10,fsize);
                if(strcmp(fid,"APIC")==0){ // cover image
                    int img_size = fsize-11; unsigned char* img = malloc(img_size);
                    memcpy(img,data+pos+11,img_size);
                    info->cover_image = img; info->cover_size = img_size;
                }
                pos+=10+fsize;
            }
            free(data);
        }
        fseek(f,-128,SEEK_END);
        unsigned char tag[128];
        if(fread(tag,1,128,f)==128 && memcmp(tag,"TAG",3)==0){
            if(strlen(info->title)==0) strncpy(info->title,(char*)tag+3,30);
            if(strlen(info->artist)==0) strncpy(info->artist,(char*)tag+33,30);
            if(strlen(info->album)==0) strncpy(info->album,(char*)tag+63,30);
        }
    } else if(strcmp(type,"FLAC")==0){
        char sig[4]; fread(sig,1,4,f);
        while(!feof(f)){
            unsigned char hdr[4]; if(fread(hdr,1,4,f)!=4) break;
            int last = hdr[0]&0x80; int btype = hdr[0]&0x7F;
            int size = (hdr[1]<<16)|(hdr[2]<<8)|hdr[3];
            if(btype==4){ // VORBIS_COMMENT
                uint32_t vlen = read_le32(f); fseek(f,vlen,SEEK_CUR);
                uint32_t count = read_le32(f);
                for(uint32_t i=0;i<count;i++){
                    uint32_t slen=read_le32(f);
                    char* entry=malloc(slen+1); fread(entry,1,slen,f); entry[slen]=0;
                    if(strncasecmp(entry,"TITLE=",6)==0) strcpy(info->title,entry+6);
                    if(strncasecmp(entry,"ARTIST=",7)==0) strcpy(info->artist,entry+7);
                    if(strncasecmp(entry,"ALBUM=",6)==0) strcpy(info->album,entry+6);
                    free(entry);
                }
                break;
            } else fseek(f,size,SEEK_CUR);
            if(last) break;
        }
    } else if(strcmp(type,"WAV")==0){
        char riff[4]; fread(riff,1,4,f);
        if(memcmp(riff,"RIFF",4)==0){
            fseek(f,12,SEEK_SET);
            char chunk[4]; uint32_t size;
            while(fread(chunk,1,4,f)==4){
                fread(&size,4,1,f);
                if(memcmp(chunk,"LIST",4)==0){
                    char type[4]; fread(type,1,4,f);
                    if(memcmp(type,"INFO",4)==0){
                        char sub[4]; uint32_t subsize;
                        if(fread(sub,1,4,f)!=4) break;
                        fread(&subsize,4,1,f);
                        if(memcmp(sub,"INAM",4)==0) fread(info->title,1,subsize,f);
                        if(memcmp(sub,"IART",4)==0) fread(info->artist,1,subsize,f);
                    } else fseek(f,size-4,SEEK_CUR);
                } else fseek(f,size,SEEK_CUR);
            }
        }
    } else if(strcmp(type,"M4A")==0){
        while(!feof(f)){
            uint32_t sz; char tp[5]={0};
            if(fread(&sz,4,1,f)!=1) break;
            if(fread(tp,4,1,f)!=1) break;
            sz=ntohl(sz);
            if(strcmp(tp,"©nam")==0) fread(info->title,1,sz-8,f);
            else if(strcmp(tp,"©ART")==0) fread(info->artist,1,sz-8,f);
            else if(strcmp(tp,"©alb")==0) fread(info->album,1,sz-8,f);
            else fseek(f,sz-8,SEEK_CUR);
        }
    }

    fclose(f);
    return 0;
}
