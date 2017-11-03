#define COEFF  16

const int csY_coeff_16 = (int) (1.164383 * (1 << COEFF));
const int csU_blue_16 = (int) (2.017232 * (1 << COEFF));
const int csU_green_16 = (int) ((-0.391762) * (1 << COEFF));
const int csV_green_16 = (int) ((-0.812968) * (1 << COEFF));
const int csV_red_16 = (int) (1.596027 * (1 << COEFF));

static uchar _color_table[256 * 5];
static const unsigned char * color_table = &_color_table[256 * 2];
static int Ym_tableEx[256];
static short Um_blue_tableEx[256];
static short Um_green_tableEx[256];
static short Vm_green_tableEx[256];
static short Vm_red_tableEx[256];

unsigned char border_color(int color) {
    if (color > 255)
        return 255;
    else if (color < 0)
        return 0;
    else
        return color;
}

static void color_table_init() {
    int i;
    for (i = 0; i < 256 * 5; ++i) {
        _color_table[i] = border_color(i - 256 * 2);
    }
}

static void yuv420_table_init() {
    int i;
    for (i = 0; i < 256; ++i) {
        if (i < 16)
            Ym_tableEx[i] = 0;
        else
            Ym_tableEx[i] = (csY_coeff_16 * (i - 16)) >> (COEFF - 3);
        
        Um_blue_tableEx[i] = (csU_blue_16 * (i - 128)) >> (COEFF - 3);
        Um_green_tableEx[i] = (csU_green_16 * (i - 128)) >> (COEFF - 3);
        Vm_green_tableEx[i] = (csV_green_16 * (i - 128)) >> (COEFF - 3);
        Vm_red_tableEx[i] = (csV_red_16 * (i - 128)) >> (COEFF - 3);
    }
}

void image_filter_initialize() {
    color_table_init();
    yuv420_table_init();
}

void YUV420SP2RGB(unsigned char* yuv420sp, unsigned char* rgb, int width, int height) {
    int j;
    int i;
    uchar r, g, b;
    unsigned int pixel_value;
    int frameSize;
    uchar* y_sp;
    uchar* uv_sp;
    y_sp = yuv420sp;
    frameSize = width * height;
    uv_sp = yuv420sp + frameSize;
    
    for (j = 0; j < height; j++) {
        int y = 0, u = 0, v = 0;
        int ye, Ue_blue, Ue_green, Ve_green, Ve_red, UeVe_green;
        
        uv_sp = yuv420sp + frameSize + (j >> 1) * width;
        
        for (i = 0; i < width; i += 2) {
            y = y_sp[0];
            v = uv_sp[0];
            u = uv_sp[1];
            ye = Ym_tableEx[y];
            
            Ue_blue = Um_blue_tableEx[u];
            Ue_green = Um_green_tableEx[u];
            Ve_green = Vm_green_tableEx[v];
            Ve_red = Vm_red_tableEx[v];
            UeVe_green = Ue_green + Ve_green;
            
            b = color_table[(ye + Ue_blue) >> 3];
            g = color_table[(ye + UeVe_green) >> 3];
            r = color_table[(ye + Ve_red) >> 3];
            
            pixel_value = (r) | (g << 8) | (b << 16) | (0xff << 24);
            
            *((unsigned int*) rgb) = pixel_value;
            
            y = y_sp[1];
            ye = Ym_tableEx[y];
            
            b = color_table[(ye + Ue_blue) >> 3];
            g = color_table[(ye + UeVe_green) >> 3];
            r = color_table[(ye + Ve_red) >> 3];
            
            pixel_value = (r) | (g << 8) | (b << 16) | (0xff << 24);
            *((unsigned int*) (rgb + 4)) = pixel_value;
            rgb += 8;
            y_sp += 2;
            uv_sp += 2;
        }
    }
}
