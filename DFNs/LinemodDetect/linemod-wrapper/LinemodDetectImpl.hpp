/**
 * @addtogroup DFNs
 * @{
 */

#ifndef LINEMODDETECT_LINEMODDETECTIMPL_HPP
#define LINEMODDETECT_LINEMODDETECTIMPL_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/rgbd/linemod.hpp>

#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace CDFF
{
namespace DFN
{
namespace LinemodDetect
{

/*!
 * @brief This class implements a 3D mesh based detector based on LINEMOD.<br>
 * This detector uses template detection in the given image. It can estimate the pose of the object by using the pose used to train the template<br>
 */
class LinemodBasedPoseDetector
{
public:
    class CPoseRecord{
        public:
            unsigned int ui_objectId;
            unsigned int ui_templateId;
            int i_bbX, i_bbY, i_bbWidth, i_bbHeight; // bounding box generated by addTemplate
            double d_R1,d_R2,d_R3; // templates rotations in rodrigues form to save mem
            double d_T1,d_T2,d_T3; // translations
    private:
           friend class boost::serialization::access; // needed by Boost
           // When the class Archive corresponds to an output archive, the
           // & operator is defined similar to <<.  Likewise, when the class Archive
           // is a type of input archive the & operator is defined similar to >>.
           template<class Archive>
           void serialize(Archive& ar, const unsigned int version)
           {
               ar & ui_objectId;
               ar & ui_templateId;
               ar & i_bbX;
               ar & i_bbY;
               ar & i_bbWidth;
               ar & i_bbHeight;
               ar & d_R1;
               ar & d_R2;
               ar & d_R3;
               ar & d_T1;
               ar & d_T2;
               ar & d_T3;
           }
     };
     typedef std::map<int, CPoseRecord> TMapRecord;
    /*!
     * @brief Constructor
     */
    LinemodBasedPoseDetector();

    /*!
     * @brief Destructor
     */
    virtual ~LinemodBasedPoseDetector();

    /*!
     * @brief Initialize as a 2D only detector (only the color gradient modality is used)
     */
    void InitAs2D(int T_level0=5, int T_level1=8);

    /*!
     * @brief Initialize as a 3D detector (the color gradient modality and depth map is used)
     */
    void InitAs3D(int T_level0=5, int T_level1=8);

    /*!
     * @brief Load a training set
     * read 2 files: one with "_training.dat" added to the argument and the other
     * with "_poses.dat" added to the argument
     * @param str_baseFullPathname Full base pathname for the files to be read
     */
    bool LoadTraining(const std::string& str_baseFullPathname);

    /*!
     * \brief Get sampling step T at pyramid_level.
     * \param pyramid_level
     * \return
     */
    int getT(int pyramid_level) const;

    /*!
     * \brief Get the template pyramid identified by template_id
     * \param class_id
     * \param template_id
     * \return
     */
    const std::vector<cv::linemod::Template>& getTemplates (const cv::String &class_id, int template_id) const;

    /*!
     * @brief Detect the training set on an imge
     * @param cvmat_image image where to perform detection
     */
    bool Detect(const std::vector<cv::Mat>& sources, float f_threshold,
                cv::linemod::Match& x_match, cv::Rect &x_bb, cv::Vec3d &vec_R, cv::Vec3d &vec_T);

    /*!
     * \brief getDetector
     * \return cv::Ptr on an object cv::linemod::Detector
     */
    cv::Ptr<cv::linemod::Detector> getDetector() { return _cvptr_detector; }

    void drawResponse(const std::vector<cv::linemod::Template>& templates,
                      int num_modalities, cv::Mat& dst, const cv::Point& offset, int T);

public:
    int _num_modalities;
    unsigned int _ui_numberOfTemplates;
    cv::Ptr<cv::linemod::Detector> _cvptr_detector;
    TMapRecord _x_poseMap;
    double _winW;
    double _winH;
    double _fx;
    double _fy;
    double _cx;
    double _cy;
};

}
}
}
#endif // LINEMODDETECT_LINEMODDETECTIMPL_HPP

/** @} */