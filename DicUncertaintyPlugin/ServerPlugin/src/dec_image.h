#include <containers/fft.h>
#include "random-standalone.h"

// DECALAGE  D'IMAGE PAR FFT
template<class T_,unsigned dim_,class Kernel_,class PT_>
ImgInterp<T_,dim_,Kernel_,PT_> dec_image(ImgInterp<T_,dim_,Kernel_,PT_> &ima_def, Vec<int,2> &xmin,
Vec<int,2> &xmax, double &depx, double &depy, bool add_noise, double &noise_level ){
    PRINT(depx);
    double pi = 3.14159265358979323846264338327950288419716939937510;
    typedef std::complex<double> C;
    typedef ImgInterp< C,dim_,Kernel_,PT_> CI;
    CI _i ; _i.resize( ima_def .size() ); _i.data = ima_def.data;
    Vec<double,dim_> siz=_i.sizes;
    CI img = fft<dim_>( _i , xmin , xmax, SymmetricPadding());
    PRINT(img.sizes);
    /// appplication d'une translation en multipliant par y |-> exp( -2 i pi < h, y > )
    for(int y=0;y<img.sizes[1];++y)
        for(int x=0;x<img.sizes[0];++x) {
            double ar;
            if ( x < img.sizes[0]/2 + 1 and y < img.sizes[1]/2 + 1 ){
                ar = 2 * pi * ( depx / double(img.sizes[0]) * x + depy / double(img.sizes[1]) * y);
            }
            if ( x > img.sizes[0]/2 and y > img.sizes[1]/2 ){
                ar = 2 * pi * ( depx / double(img.sizes[0]) * (x-img.sizes[0]) + depy / double(img.sizes[1]) *
(y-img.sizes[1]));
            }
            if ( x < img.sizes[0]/2 + 1 and y > img.sizes[1]/2 ){
                ar = 2 * pi * ( depx / double(img.sizes[0]) * x + depy / double(img.sizes[1]) * (y-img.sizes[1]));
            }
            if ( x > img.sizes[0]/2 and y < img.sizes[1]/2 + 1 ){
                ar = 2 * pi * ( depx / double(img.sizes[0]) * (x-img.sizes[0]) + depy / double(img.sizes[1]) * y);
            }
            img.tex_int( x , y ) *= C( cos( ar ), -sin( ar ) );
        }
    CI img2 = ffti( img, SymmetricPadding() );
    ImgInterp<T_,dim_,Kernel_,PT_> img_dec;img_dec.resize(img2.sizes);
    if (add_noise ){
        Random generator;
        generator.randomize(10000);
        double mean = 0;
        double sigma = noise_level * (max(ima_def.data) - min(ima_def.data)) / 100;
        PRINT(sigma);
        for( unsigned l = 0; l < img2.data.size(); ++l ){ 
            img_dec.data[ l ] = int(real( img2.data[ l ] ) + generator.gaussian(mean,sigma));
            if (img_dec.data[ l ] > 255)
                img_dec.data[ l ] = 255;
            if (img_dec.data[ l ] < 0 )
                img_dec.data[ l ] = 0;
        }
    }
    else{
        for( unsigned l = 0; l < img2.data.size(); ++l ){ 
            img_dec.data[ l ] = int(real( img2.data[ l ] ));
            if (img_dec.data[ l ] > 255)
                img_dec.data[ l ] = 255;
            if (img_dec.data[ l ] < 0 )
                img_dec.data[ l ] = 0;
        }
    }
    return img_dec;
}
