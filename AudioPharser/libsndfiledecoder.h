#ifndef LIBSNDFILEDECODER_H
#define LIBSNDFILEDECODER_H

#include <QObject> 
#include <sndfile.h> 
#include <QDebug> 


class SndFileDecoder : public QObject { 
    Q_OBJECT 

public:
    typedef struct {
        SNDFILE     *file;
        SF_INFO      Fileinfo;
        sf_count_t   currentframe; 
    } SndfileHandler;
    SndFileDecoder();
    ~SndFileDecoder();

    bool SndfileSetFile(const char* filename);
    sf_count_t GetCurrentFrame();
    sf_count_t SndfileReadFloat(float* outBuffer, int framesperbuffer);
    sf_count_t SetSampleTo(sf_count_t targetFrame);
    void SndfileCloseFile();


    const SF_INFO& getFileInfo() const { return sndfilehandler.Fileinfo; }
    sf_count_t getLogicalCurrentFrame() const { return sndfilehandler.currentframe; }


private:
    SndfileHandler sndfilehandler;

};

#endif // LIBSNDFILEDECODER_H