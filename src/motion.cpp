#include <iostream>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <time.h>
#include <dirent.h>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace cv;

// Vérifie si le repo existe
// Créer un nouveau repo si l'image est la première
inline void directoryExistsOrCreate(const char* pzPath)
{
    DIR *pDir;
    // si repo existe pas, création
    if ( pzPath == NULL || (pDir = opendir (pzPath)) == NULL)
        mkdir(pzPath, 0777);
    else if(pDir != NULL)
        (void) closedir (pDir);
}

// quand un mouvement est detecté on la sauvegarde
int incr = 0;
inline bool saveImg(Mat image, const string DIRECTORY, const string EXTENSION, const char * DIR_FORMAT, const char * FILE_FORMAT)
{
    stringstream ss;
    time_t seconds;
    struct tm * timeinfo;
    char TIME[80];
    time (&seconds);
    timeinfo = localtime (&seconds);
    
    strftime (TIME,80,DIR_FORMAT,timeinfo);
    ss.str("");
    ss << DIRECTORY << TIME;
    directoryExistsOrCreate(ss.str().c_str());
   
    // nom pour l'image
    strftime (TIME,80,FILE_FORMAT,timeinfo);
    ss.str("");
    if(incr < 100) incr++;
    else incr = 0;
    ss << DIRECTORY << TIME << static_cast<int>(incr) << EXTENSION;
    return imwrite(ss.str().c_str(), image);
}

// Vérifie si il y a des mouvements dans la matrice résultante
// Compte le nombres de changements et le retourne
inline int detectMotion(const Mat & motion, Mat & result, Mat & result_cropped,
                 int x_start, int x_stop, int y_start, int y_stop,
                 int max_deviation,
                 Scalar & color)
{
    Scalar mean, stddev;
    meanStdDev(motion, mean, stddev);
    // si pas assez de changement, alors c'est l'image originel
    if(stddev[0] < max_deviation)
    {
        int number_of_changes = 0;
        int min_x = motion.cols, max_x = 0;
        int min_y = motion.rows, max_y = 0;
        // parcours l'image pour détecter les changements
        for(int j = y_start; j < y_stop; j+=2){ // height
            for(int i = x_start; i < x_stop; i+=2){ // width
                // si l'intensité est égale a 255, le pixel est différent
                if(static_cast<int>(motion.at<uchar>(j,i)) == 255)
                {
                    number_of_changes++;
                    if(min_x>i) min_x = i;
                    if(max_x<i) max_x = i;
                    if(min_y>j) min_y = j;
                    if(max_y<j) max_y = j;
                }
            }
        }
        if(number_of_changes){
            //check si on est pas en dehors des limites
            if(min_x-10 > 0) min_x -= 10;
            if(min_y-10 > 0) min_y -= 10;
            if(max_x+10 < result.cols-1) max_x += 10;
            if(max_y+10 < result.rows-1) max_y += 10;

        }
        return number_of_changes;
    }
    return 0;
}

int main (int argc, char * const argv[])
{
    const string DIR = "/home/pi/Projects/raspwatcher/pics";
    const string EXT = ".jpg";
    const int DELAY = 100; 
    const string LOGFILE = "/home/pi/Projects/raspwatcher/logs";

    // Format of directory
    string DIR_FORMAT = "";
    string FILE_FORMAT = DIR_FORMAT + "/" + "%d%h%Y_%H%M%S"; // 1Jan1970/1Jan1970_12153

    // configure la caméra
    VideoCapture camera(0);
    if(!camera.isOpened())
            return -1;
    std::cout << "Camera ok Début de la capture..." << std::endl;
    camera.set(CV_CAP_PROP_FRAME_WIDTH, 800);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    
    // prend une photo et la converti en gris
    Mat result, result_cropped, current_frame, next_frame;
    camera.read(result);
    Mat prev_frame = result;
    camera.read(current_frame);
    camera.read(next_frame);
    cvtColor(current_frame, current_frame, CV_RGB2GRAY);
    cvtColor(prev_frame, prev_frame, CV_RGB2GRAY);
    cvtColor(next_frame, next_frame, CV_RGB2GRAY);
    
    Mat d1, d2, motion;
    int number_of_changes, number_of_sequence = 0;
    Scalar mean_, color(0,255,255); // yellow
    
    int x_start = 10, x_stop = current_frame.cols-11;
    int y_start = 350, y_stop = 530;

    // Si il y a plus de pixel changé que there_is_motion, il ya a un mouvement
    // et sauvegarde l'image sur le disque
    int there_is_motion = 5;
    
    // plus la déviation est elevé plus le seuil de detcion l'est aussi
    int max_deviation = 20;
    
    Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2,2));
    
    //Début de la boucle inifnie 
    while (true){
        //prend une photo
        prev_frame = current_frame;
        current_frame = next_frame;
        camera.read(next_frame);
        result = next_frame;
        cvtColor(next_frame, next_frame, CV_RGB2GRAY);

        // Calcule la différence entre les images en faisant un and
        // de ce fait les faible difference de contrast sont ignorés
        absdiff(prev_frame, next_frame, d1);
        absdiff(next_frame, current_frame, d2);
        bitwise_and(d1, d2, motion);
        threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
        erode(motion, motion, kernel_ero);
        
        number_of_changes = detectMotion(motion, result, result_cropped,  x_start, x_stop, y_start, y_stop, max_deviation, color);
        
        // changement
        if(number_of_changes>=there_is_motion)
        {
            if(number_of_sequence>0){ 
                std::cout << "Mouvement detecté, photo enregistré..." << std::endl;
                saveImg(result,DIR,EXT,DIR_FORMAT.c_str(),FILE_FORMAT.c_str());
            }
            number_of_sequence++;
        }
        else
        {
            number_of_sequence = 0;
            cvWaitKey (DELAY);
        }
    }
    return 0;    
}
