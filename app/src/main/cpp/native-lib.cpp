#include <jni.h>
#include <string>
#include <opencv2/core.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/core/ocl.hpp>
#include <ceres/ceres.h>
#include <Eigen/Dense>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>


#include <android/log.h>
#define LOG_TAG "openclTest"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
using namespace cv;
using namespace std;
extern int vins_estimator_main(const string &config, const string &imageData, const string &timestamp, const string & imuData );

void testEigen()
{
    using namespace Eigen;

    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    LOGD("Here is the matrix m: %f, %f, %f, %f " , m(0,0),m(0,1),m(1,0),m(1,1) );
    VectorXd v(2);
    v(0) = 4;
    v(1) = v(0) - 1;
    LOGD("Here is the vector v: %f, %f" , v(0), v(1));
    // std::cout << "Here is the vector v:\n" << v << std::endl;


}
void testBoost()
{
    using boost::lexical_cast;
    int a = lexical_cast<int>("123");
    std::string inputDir = "../include";
    double b = lexical_cast<double>("123.12");
    LOGD("A: %d",a);
    LOGD("B: %lf",b);
    if (!boost::filesystem::exists(inputDir) && !boost::filesystem::is_directory(inputDir));
        LOGD("filesystem test pass");
}
#if 1
struct CostFunctor {
    template <typename T>
    bool operator()(const T* const x, T* residual) const {
        residual[0] = T(10.0) - x[0];
        return true;
    }
};

void testCeres(void)
{
//using namespace ceres;
// The variable to solve for with its initial value.
    double initial_x = 5.0;
    double x = initial_x;

    // Build the problem.
    ceres::Problem problem;

    // Set up the only cost function (also known as residual). This uses
    // auto-differentiation to obtain the derivative (jacobian).
    ceres::CostFunction* cost_function =
            new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
    problem.AddResidualBlock(cost_function, NULL, &x);

    // Run the solver!
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);
 //   LOGD("%s",summary.BriefReport() );
    LOGD("X: %f -> %f",initial_x,x);

//    std::cout << summary.BriefReport() << "\n";
  //  std::cout << "x : " << initial_x
 //   << " -> " << x << "\n";
}
#endif
void testOcl(void)
{

    try {
        if (!cv::ocl::haveOpenCL()){
            LOGD("OpenCL is not availble");
        } else{
            LOGD("OpenCL is avaible");
        }
        if (cv::ocl::useOpenCL()){
            LOGD("use OpenCL");
        } else{
            LOGD("don't use OpenCL");
        }
        cv::ocl::Context context;
        if (!context.create(cv::ocl::Device::TYPE_GPU))
        {
            LOGD("Failed creating the context...");
            return;
        }
        else
        {
            LOGD("ocl::Context is OK");
        }

      //  LOGD(" %lu GPU devices are detected.",context.ndevices());
        for (int i = 0; i < context.ndevices(); i++)
        {
            cv::ocl::Device device = context.device(i);
            LOGD("name: %s",device.name().c_str());
            if (device.available()){
                LOGD("device is avaible");
            } else{
                LOGD("devive is not avaible");
            }
            if (device.imageSupport()){
                LOGD("device support image");
            } else{
                LOGD("device doesn't support image");
            }
            LOGD("OpenCL_C_Version     : %s" ,device.OpenCL_C_Version().c_str());
        }
    }
    catch(cv::Error::Code& e)
    {
        LOGE("cv::Error::Code %d", e);
    }

}

extern "C" {
jstring
Java_com_hezhaoxin_solomon_vins_1android_NDKloader_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "opencv图像处理";
    return env->NewStringUTF(hello.c_str());
}
jstring
Java_com_hezhaoxin_solomon_vins_1android_NDKloader_validate(
        JNIEnv *env,
        jobject,
        jlong addrGray,
        jlong addrRgba){
    std::string hello2 = "Hello from validate";
    return env->NewStringUTF(hello2.c_str());
}
}
//图像处理
extern "C"
JNIEXPORT jintArray JNICALL
Java_com_hezhaoxin_solomon_vins_1android_NDKloader_getGrayImage(
        JNIEnv *env,
        jclass type,
        jintArray pixels_,
        jint w, jint h) {
    jint *pixels = env->GetIntArrayElements(pixels_, NULL);
    // TODO
    if(pixels==NULL){
        return NULL;
    }
    cv::Mat imgData(h, w, CV_8UC4, pixels);
    uchar *ptr = imgData.ptr(0);
    for (int i = 0; i < w * h; i++) {
        int grayScale = (int) (ptr[4 * i + 2] * 0.299 + ptr[4 * i + 1] * 0.587
                               + ptr[4 * i + 0] * 0.114);
        ptr[4 * i + 1] = (uchar) grayScale;
        ptr[4 * i + 2] = (uchar) grayScale;
        ptr[4 * i + 0] = (uchar) grayScale;
    }

    int size = w * h;
    jintArray result = env->NewIntArray(size);
    env->SetIntArrayRegion(result, 0, size, pixels);
    env->ReleaseIntArrayElements(pixels_, pixels, 0);
      testOcl();
    //  LOGuD("HELLO WORLD");
    testEigen();
    testCeres();
    testBoost();
     LOGD("TEST LOG");
    vins_estimator_main("/data/vins/euroc_config.yaml", "/data/vins/data/image/MH_01_easy/data", "/data/vins/data/timestamps/MH01.txt", "/data/vins/data/imu/data.csv" );
    return result;
}
