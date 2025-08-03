#include "libsndfiledecoder.h" 


static int checkFileOnErrors(SNDFILE *file) {
    if (sf_error(file) != 0) {
        qCritical() << "Sndfile Error!: " << sf_strerror(file);
        return sf_error(file);
    }
    return 0;
}

SndFileDecoder::SndFileDecoder() {
    sndfilehandler.file = nullptr; 
    memset(&sndfilehandler.Fileinfo, 0, sizeof(SF_INFO)); 
    sndfilehandler.currentframe = 0;
}

SndFileDecoder::~SndFileDecoder() {
    SndfileCloseFile(); 
}

bool SndFileDecoder::SndfileSetFile(const char* filename) {
    SndfileCloseFile(); 

    
    sndfilehandler.file = sf_open(filename, SFM_READ, &sndfilehandler.Fileinfo);

    if (!sndfilehandler.file) {
        
        qCritical() << "Failed to open file: " << filename << " - " << sf_strerror(nullptr);
        return false;
    }

    
    if (checkFileOnErrors(sndfilehandler.file) != 0) {
        SndfileCloseFile(); 
        return false;
    }

    
    sndfilehandler.currentframe = 0;
    return true;
}

sf_count_t SndFileDecoder::GetCurrentFrame() {
    if (!sndfilehandler.file) {
        qWarning() << "Cannot get current frame: no file loaded.";
        return -1; 
    }

    
    sf_count_t current_pos = sf_seek(sndfilehandler.file, 0, SF_SEEK_CUR);
    if (current_pos == -1) {
        qWarning() << "Error retrieving current frame position: " << sf_strerror(sndfilehandler.file);
        return -1; 
    }
    
    sndfilehandler.currentframe = current_pos;
    return sndfilehandler.currentframe;
}

sf_count_t SndFileDecoder::SndfileReadFloat(float* outBuffer, int framesperbuffer) {
    if (!sndfilehandler.file || !outBuffer) {
        qWarning() << "SndfileReadFloat: Invalid file handle or output buffer.";
        return 0; 
    }

    sf_count_t frames_to_read = static_cast<sf_count_t>(framesperbuffer * sndfilehandler.Fileinfo.channels);
    sf_count_t frames_read = sf_read_float(sndfilehandler.file, outBuffer, frames_to_read);

    if (frames_read < 0) {
        qWarning() << "SndfileReadFloat: Error reading frames: " << sf_strerror(sndfilehandler.file);
        return 0; 
    }


    sndfilehandler.currentframe += (frames_read / sndfilehandler.Fileinfo.channels);

    return frames_read; 
}

sf_count_t SndFileDecoder::SetSampleTo(sf_count_t targetFrame) {
    if (!sndfilehandler.file || sndfilehandler.Fileinfo.frames <= 0) {
        qWarning() << "Cannot seek: no file loaded or file has no frames.";
        return -1; 
    }

   
    sf_count_t seek_result = sf_seek(sndfilehandler.file, targetFrame, SF_SEEK_SET);

    if (seek_result == -1) {
        qWarning() << "Error seeking in file: " << sf_strerror(sndfilehandler.file);
        return -1; 
    } else {
        sndfilehandler.currentframe = seek_result;
        return seek_result; 
    }
}

void SndFileDecoder::SndfileCloseFile() {
    if (sndfilehandler.file) {
        sf_close(sndfilehandler.file);
        sndfilehandler.file = nullptr; 
        memset(&sndfilehandler.Fileinfo, 0, sizeof(SF_INFO));
        sndfilehandler.currentframe = 0;
    }
}