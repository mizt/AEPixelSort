namespace AEPixelSort {
  
  // https://ja.wikipedia.org/wiki/%E3%82%AF%E3%82%A4%E3%83%83%E3%82%AF%E3%82%BD%E3%83%BC%E3%83%88
  
  inline unsigned int med3(unsigned int x, unsigned int y, unsigned int z) {
    if(x<y) {
      if(y<z) return y;
      else if(z<x) return x;
    }
    else {
      if(z<y) return y;
      else if(x<z) return x;
    }

    return z;
  } 
        
  void quicksort(unsigned int *a, int left, int right) {
    if(left<right) {
      int i = left, j = right;
      unsigned int tmp, pivot = med3(a[i],a[i+((j-i)>>1)],a[j]);
      while(1) {
        while(a[i] < pivot) i++;
        while(pivot < a[j]) j--;
        if(i >= j) break;
        tmp = a[i]; a[i] = a[j]; a[j] = tmp;
        i++; j--;
      }
      quicksort(a,left,i-1);
      quicksort(a,j+1,right);
    }
  }
  
  class Buffer {
    private:
    
      unsigned int *_data = nullptr;
      unsigned int _length = 0;
    
      void free() {
        if(_data!=nullptr) {
          delete[] _data;
          _data = nullptr;
        }
      }
    
    public:
    
      Buffer() {}
    
      unsigned int *data() { return _data; }
      unsigned int length() { return _length; }
    
      void realloc(unsigned int len) {
        
        if(len>_length) {
          
          this->free();
          _length = len;
          _data = new unsigned int[_length];
        }
      }
    
      ~Buffer() {
        this->free();
      }
  };
  
  class Object {
  
    private:
    
      unsigned int _width = 0;
      unsigned int _height = 0;
    
      Buffer *_buffer = new Buffer();
      Buffer *_line = new Buffer();

      Object() {
      }
    
      Object(const Object &p) {}
      virtual ~Object() {}
    
    public:
    
      unsigned int *buffer() { return _buffer->data(); };
      unsigned int *line() { return _line->data(); };
    
      void sort(int left, int right) {
          quicksort(_line->data(),left,right);
      }
    
      void setup(int width,int height) {
        _buffer->realloc(width*height);
        _line->realloc(width>height?width:height);
      }
    
      unsigned int width() {
        return _width;
      }
    
      unsigned int height() {
        return _height;
      }
    
      static Object *getInstance() {
        static Object instance;
        return &instance;
      }
  };
  
  Object *$() {
    return Object::getInstance();
  }
  
  // A C implementation of Kim Asendorf's PixelSort
  // https://github.com/kimasendorf/ASDFPixelSort
  
  
  inline unsigned int gris(unsigned char r,unsigned char g,unsigned char b) {
      unsigned char tmp = (306*r+601*g+116*b)>>10;
      return tmp<<16|tmp<<8|tmp;
  }
  
  void render(PF_EffectWorld *input,PF_LayerDef	*output,PF_ParamDef *params[]) {

    bool isBrightness = ((PF_Boolean)params[PIXELSORT_BRIGHTNESS]->u.bd.value==TRUE)?true:false;
    unsigned int x = params[PIXELSORT_X]->u.sd.value;
    unsigned int y = params[PIXELSORT_Y]->u.sd.value;
    
    x |= x<<16|x<<8;
    y |= y<<16|y<<8;
    
    int width  = output->width;
    int height = output->height;
    
    
    unsigned int *src = (unsigned int *)input->data;
    int srcRow = input->rowbytes>>2;
    
    unsigned int *dst = (unsigned int *)output->data;
    int dstRow = output->rowbytes>>2;
    
    
    if(x==0&&y==0) {
      
      for(int i=0; i<height; i++) {
        for(int j=0; j<width; j++) {
          dst[i*dstRow+j] = src[i*srcRow+j];
        }
      }
    }
    else {
    
      AEPixelSort::$()->setup(width,height);
      
      unsigned int bgr = 0;
      unsigned int tmp = 0;
      
      int then = 0;
      bool state = false;

      unsigned char r;
      unsigned char g;
      unsigned char b;
      
      unsigned int *line = AEPixelSort::$()->line();
      
      if(y) {
        
        if(x) {
          dst = AEPixelSort::$()->buffer();
          dstRow = width;
        }
      
        
        for(int j=0; j<width; j++) {
            
          then = 0;
          state = false;
            
          for(int i=0; i<height; i++) {
               
            bgr = src[i*srcRow+j]>>8;
            
            r = bgr&0xFF;
            g = (bgr&0xFF00)>>8;
            b = (bgr>>16);
                
            tmp = line[i] = r<<16|g<<8|b;
            if(isBrightness) {
              tmp = gris(r,b,g);
            }
            
            if(state) {
              if(tmp>y) {
                AEPixelSort::$()->sort(then,i);
                state = false;
              }
            }
            else {
              if(tmp<y) {
                then = i;
                state = true;
              }
            }
          }
            
          if(state&&then!=height-1) AEPixelSort::$()->sort(then,height);
          
          for(int i=0; i<height; i++) {
            tmp = line[i];
            dst[i*dstRow+j] = (((tmp>>16)|(tmp&0xFF00)|((tmp&0xFF)<<16))<<8)|0xFF;
          }
        }
      }
     
      if(x) {
      
        if(y)  {
          src = AEPixelSort::$()->buffer();
          srcRow = width;
        }
      
        dst = (unsigned int *)output->data;
        dstRow = output->rowbytes>>2;
        
        for(int i=0; i<height; i++) {
            
          then = 0;
          state = false;
            
          for(int j=0; j<width; j++) {
               
            bgr = src[i*srcRow+j]>>8;
             
            r = bgr&0xFF;
            g = (bgr&0xFF00)>>8;
            b = (bgr>>16);
            tmp = line[j] = r<<16|g<<8|b;
            if(isBrightness) {
              tmp = gris(r,b,g);
            }
            
            if(state) {
              if(tmp>x) {
                AEPixelSort::$()->sort(then,j);
                state = false;
              }
            }
            else {
              if(tmp<x) {
                then = j;
                state = true;
              }
            }
          }
            
          if(state&&then!=width-1) AEPixelSort::$()->sort(then,width);
            
          for(int j=0; j<width; j++) {
              tmp = line[j];
              dst[i*dstRow+j] = (((tmp>>16)|(tmp&0xFF00)|((tmp&0xFF)<<16))<<8)|0xFF;
          }
        }
      }
    }
  }
}
