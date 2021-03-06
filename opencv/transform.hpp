#ifndef _HTL_TRANSFORM_HPP__
#define _HTL_TRANSFORM_HPP__

#include <opencv2/opencv.hpp>
#include "pose.hpp"

namespace htl
{
    class Transform
    {
    public:
        template <class T>
        static bool RotMatToQuaternion(T *qx, T *qy, T *qz, T *qw,
                                       const T &m11, const T &m12, const T &m13,
                                       const T &m21, const T &m22, const T &m23,
                                       const T &m31, const T &m32, const T &m33);
        template <class T>
        static bool RotMatToQuaternion(const cv::Mat &RotMat,
                                       T *qx, T *qy, T *qz, T *qw);

        template <class T>
        static void QuaternionToEulerAngles(T q0, T q1, T q2, T q3,
                                            T &roll, T &pitch, T &yaw);

        template <class T>
        static cv::Vec<T, 3> QuaternionToRodrigues(T q0, T q1, T q2, T q3);

        template <class T>
        static void QuaternionToRotMat(T &m11, T &m12, T &m13,
                                       T &m21, T &m22, T &m23,
                                       T &m31, T &m32, T &m33,
                                       const T &qx, const T &qy, const T &qz, const T &qw);

        template <class T>
        static cv::Mat QuaternionToRotMat(const T &qx, const T &qy, const T &qz, const T &qw);

        template <class T>
        static cv::Mat QuaternionToRotMat(const htl::Quaternion<T> &quaternion);

        template <class T>
        static void EulerAnglesToQuaternion(T roll, T pitch, T yaw,
                                            T &q0, T &q1, T &q2, T &q3);
        template <class T>
        static T RevFromRotMat(const cv::Mat &Rotation);

        template <class T>
        static void RotMatToAngles(const cv::Mat &R, T &angle_x, T &angle_y, T &angle_z);

        template <class T>
        static void RodriguesToQuaternion(const cv::Vec<T, 3> &rvec,
                                          T &q0, T &q1, T &q2, T &q3);
    };
} // namespace htl

template <class T>
bool htl::Transform::RotMatToQuaternion(T *qx, T *qy, T *qz, T *qw, const T &m11,
                                        const T &m12, const T &m13, const T &m21,
                                        const T &m22, const T &m23, const T &m31,
                                        const T &m32, const T &m33)

{
    // 最大成分を検索
    T elem[4]; // 0:x, 1:y, 2:z, 3:w
    elem[0] = m11 - m22 - m33 + 1.0;
    elem[1] = -m11 + m22 - m33 + 1.0;
    elem[2] = -m11 - m22 + m33 + 1.0;
    elem[3] = m11 + m22 + m33 + 1.0;

    unsigned biggestIndex = 0;
    for (int i = 1; i < 4; i++)
    {
        if (elem[i] > elem[biggestIndex])
            biggestIndex = i;
    }

    if (elem[biggestIndex] < 0.0f)
        return false; // 引数の行列に間違いあり！

    // 最大要素の値を算出
    T *q[4] = {qx, qy, qz, qw};
    T v = std::sqrt(elem[biggestIndex]) * 0.5f;
    *q[biggestIndex] = v;
    T mult = 0.25f / v;

    switch (biggestIndex)
    {
    case 0: // x
        *q[1] = (m12 + m21) * mult;
        *q[2] = (m31 + m13) * mult;
        *q[3] = (m32 - m23) * mult;
        break;
    case 1: // y
        *q[0] = (m12 + m21) * mult;
        *q[2] = (m23 + m32) * mult;
        *q[3] = (m13 - m31) * mult;
        break;
    case 2: // z
        *q[0] = (m31 + m13) * mult;
        *q[1] = (m23 + m32) * mult;
        *q[3] = (m21 - m12) * mult;
        break;
    case 3: // w
        *q[0] = (m32 - m23) * mult;
        *q[1] = (m13 - m31) * mult;
        *q[2] = (m21 - m12) * mult;
        break;
    }

    return true;
}

template <class T>
bool htl::Transform::RotMatToQuaternion(const cv::Mat &R,
                                        T *qx, T *qy, T *qz, T *qw)

{
    T m11 = R.at<T>(0, 0);
    T m12 = R.at<T>(0, 1);
    T m13 = R.at<T>(0, 2);
    T m21 = R.at<T>(1, 0);
    T m22 = R.at<T>(1, 1);
    T m23 = R.at<T>(1, 2);
    T m31 = R.at<T>(2, 0);
    T m32 = R.at<T>(2, 1);
    T m33 = R.at<T>(2, 2);

    // 最大成分を検索
    T elem[4]; // 0:x, 1:y, 2:z, 3:w
    elem[0] = m11 - m22 - m33 + 1.0;
    elem[1] = -m11 + m22 - m33 + 1.0;
    elem[2] = -m11 - m22 + m33 + 1.0;
    elem[3] = m11 + m22 + m33 + 1.0;

    unsigned biggestIndex = 0;
    for (int i = 1; i < 4; i++)
    {
        if (elem[i] > elem[biggestIndex])
            biggestIndex = i;
    }

    if (elem[biggestIndex] < 0.0f)
        return false; // 引数の行列に間違いあり！

    // 最大要素の値を算出
    T *q[4] = {qx, qy, qz, qw};
    T v = std::sqrt(elem[biggestIndex]) * 0.5f;
    *q[biggestIndex] = v;
    T mult = 0.25f / v;

    switch (biggestIndex)
    {
    case 0: // x
        *q[1] = (m12 + m21) * mult;
        *q[2] = (m31 + m13) * mult;
        *q[3] = (m32 - m23) * mult;
        break;
    case 1: // y
        *q[0] = (m12 + m21) * mult;
        *q[2] = (m23 + m32) * mult;
        *q[3] = (m13 - m31) * mult;
        break;
    case 2: // z
        *q[0] = (m31 + m13) * mult;
        *q[1] = (m23 + m32) * mult;
        *q[3] = (m21 - m12) * mult;
        break;
    case 3: // w
        *q[0] = (m32 - m23) * mult;
        *q[1] = (m13 - m31) * mult;
        *q[2] = (m21 - m12) * mult;
        break;
    }

    return true;
}

template <class T>
void htl::Transform::QuaternionToEulerAngles(T q0, T q1, T q2, T q3, T &roll,
                                             T &pitch, T &yaw)
{
    T q0q0 = q0 * q0;
    T q0q1 = q0 * q1;
    T q0q2 = q0 * q2;
    T q0q3 = q0 * q3;
    T q1q1 = q1 * q1;
    T q1q2 = q1 * q2;
    T q1q3 = q1 * q3;
    T q2q2 = q2 * q2;
    T q2q3 = q2 * q3;
    T q3q3 = q3 * q3;
    roll = std::atan2(2.0 * (q2q3 + q0q1), q0q0 - q1q1 - q2q2 + q3q3);
    pitch = std::asin(2.0 * (q0q2 - q1q3));
    yaw = std::atan2(2.0 * (q1q2 + q0q3), q0q0 + q1q1 - q2q2 - q3q3);
}

template <class T>
cv::Vec<T, 3> htl::Transform::QuaternionToRodrigues(T q0, T q1, T q2, T q3)
{
    cv::Mat R = QuaternionToRotMat<T>(q0, q1, q2, q3);
    cv::Vec<T, 3> rvec;
    cv::Rodrigues(R, rvec);
    return rvec;
}

template <class T>
void htl::Transform::RodriguesToQuaternion(const cv::Vec<T, 3> &rvec,
                                           T &q0, T &q1, T &q2, T &q3)
{
    cv::Mat R = (cv::Mat_<T>(3, 3) << 1., 0., 0.,
                 0., 1., 0.,
                 0., 0., 1.);
    cv::Rodrigues(rvec, R);
    RotMatToQuaternion<T>(R, &q0, &q1, &q2, &q3);
}

template <class T>
void htl::Transform::EulerAnglesToQuaternion(T roll, T pitch, T yaw,
                                             T &q0, T &q1, T &q2, T &q3)
{
    T cosRoll = std::cos(roll / 2.0);
    T sinRoll = std::sin(roll / 2.0);
    T cosPitch = std::cos(pitch / 2.0);
    T sinPitch = std::sin(pitch / 2.0);
    T cosYaw = std::cos(yaw / 2.0);
    T sinYaw = std::sin(yaw / 2.0);

    q0 = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;
    q1 = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    q2 = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    q3 = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
}

template <class T>
void htl::Transform::QuaternionToRotMat(T &m11, T &m12, T &m13, T &m21, T &m22,
                                        T &m23, T &m31, T &m32, T &m33,
                                        const T &qx, const T &qy, const T &qz, const T &qw)
{
    m11 = 1.0f - 2.0 * qy * qy - 2.0 * qz * qz;
    m12 = 2.0 * qx * qy + 2.0 * qw * qz;
    m13 = 2.0 * qx * qz - 2.0 * qw * qy;

    m21 = 2.0 * qx * qy - 2.0 * qw * qz;
    m22 = 1.0f - 2.0 * qx * qx - 2.0 * qz * qz;
    m23 = 2.0 * qy * qz + 2.0 * qw * qx;

    m31 = 2.0 * qx * qz + 2.0 * qw * qy;
    m32 = 2.0 * qy * qz - 2.0 * qw * qx;
    m33 = 1.0f - 2.0 * qx * qx - 2.0 * qy * qy;
}

template <class T>
cv::Mat htl::Transform::QuaternionToRotMat(const T &qx, const T &qy, const T &qz, const T &qw)
{
    cv::Mat Output = cv::Mat_<T>(3, 3);
    Output.at<T>(0, 0) = qx * qx - qy * qy - qz * qz + qw * qw;
    Output.at<T>(0, 1) = 2.0 * (qx * qy - qz * qw);
    Output.at<T>(0, 2) = 2.0 * (qx * qz + qy * qw);
    Output.at<T>(1, 0) = 2.0 * (qx * qy + qz * qw);
    Output.at<T>(1, 1) = -qx * qx + qy * qy - qz * qz + qw * qw;
    Output.at<T>(1, 2) = 2.0 * (qy * qz - qx * qw);
    Output.at<T>(2, 0) = 2.0 * (qx * qz - qy * qw);
    Output.at<T>(2, 1) = 2.0 * (qy * qz + qx * qw);
    Output.at<T>(2, 2) = -qx * qx - qy * qy + qz * qz + qw * qw;
    return Output;
}

template <class T>
cv::Mat htl::Transform::QuaternionToRotMat(const htl::Quaternion<T> &quaternion)
{
    T qx = quaternion.x;
    T qy = quaternion.y;
    T qz = quaternion.z;
    T qw = quaternion.w;
    cv::Mat Output = cv::Mat_<T>(3, 3);
    Output.at<T>(0, 0) = qx * qx - qy * qy - qz * qz + qw * qw;
    Output.at<T>(0, 1) = 2.0 * (qx * qy - qz * qw);
    Output.at<T>(0, 2) = 2.0 * (qx * qz + qy * qw);
    Output.at<T>(1, 0) = 2.0 * (qx * qy + qz * qw);
    Output.at<T>(1, 1) = -qx * qx + qy * qy - qz * qz + qw * qw;
    Output.at<T>(1, 2) = 2.0 * (qy * qz - qx * qw);
    Output.at<T>(2, 0) = 2.0 * (qx * qz - qy * qw);
    Output.at<T>(2, 1) = 2.0 * (qy * qz + qx * qw);
    Output.at<T>(2, 2) = -qx * qx - qy * qy + qz * qz + qw * qw;
    return Output;
}

template <class T>
T htl::Transform::RevFromRotMat(const cv::Mat &Rotation)
{
    //回転行列をクォータニオンに変換
    T qx, qy, qz, qw;
    RotMatToQuaternion<T>(Rotation, &qx, &qy, &qz, &qw);
    //クォータニオンの4つめの要素から回転角を取り出す
    T phi = 2. * std::acos(qw);
    return phi;
}

template <class T>
void htl::Transform::RotMatToAngles(const cv::Mat &R, T &angle_x, T &angle_y,
                                    T &angle_z)
{
    T threshhold = 0.001;

    if (std::abs(R.at<T>(2, 1) - 1.0) < threshhold)
    {
        angle_x = M_PI / 2;
        angle_y = 0;
        angle_z = std::atan2(R.at<T>(1, 0), R.at<T>(0, 0));
    }
    else if (std::abs(R.at<T>(2, 1) + 1.0) < threshhold)
    {
        angle_x = -M_PI / 2;
        angle_y = 0;
        angle_z = std::atan2(R.at<T>(1, 0), R.at<T>(0, 0));
    }
    else
    {
        angle_x = std::asin(R.at<T>(2, 1));
        angle_y = std::atan2(R.at<T>(2, 0), R.at<T>(2, 2));
        angle_z = std::atan2(R.at<T>(0, 1), R.at<T>(1, 1));
    }
}
#endif