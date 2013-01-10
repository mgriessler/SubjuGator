#ifndef NAVIGATIONCOMPUTER_H
#define NAVIGATIONCOMPUTER_H

#include <Eigen/Dense>

#include "EigenUtils.h"

#include "DataObjects/DVLVelocity.h"
#include "DataObjects/IMUInfo.h"
#include "DataObjects/DepthInfo.h"
#include "DataObjects/LPOSVSSInfo.h"
#include "INS.h"
#include "Kalman.h"

namespace subjugator {
	class NavigationComputer {
	public:
		struct Config {
			Vector3d referenceNorthVector;
			double latitudeDeg;

			Vector3d dvl_sigma;
			Vector3d att_sigma;
		};

		NavigationComputer(const Config &config);
		bool getInitialized() { return initialized; }
		void UpdateIMU(const IMUInfo& imu);
		void UpdateDepth(const DepthInfo& dobj);
		void UpdateDVL(const DVLVelocity& dvl);

		void GetNavInfo(LPOSVSSInfo& info);

		void Update(boost::int64_t dtms);

	private:
		Config conf;

		static const double latitudeDeg = 29.651388889; /*gainesville*/
		static const double alpha = 0.4082;
		static const double beta = 2.0;
		static const double kappa = 0;
		static const double bias_var_f = 0.000004;
		static const double bias_var_w = 26.2;
		static const double T_f = 5; // TODO
		static const double T_w = 100; // TODO
		static const double depth_sigma = 0.02;

		static const double MAX_DEPTH = 15; // m
		static const double MAX_DVL_NORM = 10; // Sub can't run at 10m/s

		Vector3d referenceGravityVector;
		Vector3d initialPosition;
		Vector3d initialVelocity;
		Vector3d white_noise_sigma_f;
		Vector3d white_noise_sigma_w;
		Vector4d q_SUB_DVL;
		Vector4d q_SUB_IMU;
		Eigen::Matrix<double, 13, 13> covariance;

		bool depthRefAvailable;
		bool attRefAvailable;
		bool velRefAvailable;
		double depthRef;
		Vector4d attRef;
		Vector3d velRef;
		Vector7d z;
		Vector3d r_ORIGIN_NAV;

		Vector3d magSum;
		Vector3d accSum;
		double acceptable_gravity_mag;

		std::auto_ptr<KalmanFilter> kFilter;
		std::auto_ptr<INS> ins;

		bool initialized;

		int kalmanCount;
		boost::int64_t kTimerMs;
		boost::int64_t kTimer;

		void TryInit(const IMUInfo &imuInfo);
		void updateKalman();
		boost::int64_t getTimestamp(void);
		void resetErrors();
	};
}

#endif /* NAVIGATIONCOMPUTER_H */