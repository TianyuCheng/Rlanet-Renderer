#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

/**
 * This file is copied from CS384G's raytracer assignment with 
 * minimal changes to port to Qt. Thanks to anyone who wrote the
 * code.
 * */

#include <QVector3D>

class BoundingBox {
	bool bEmpty;
	bool dirty;
	QVector3D bmin;
	QVector3D bmax;
	double bArea;
	double bVolume;

    QVector3D corners[8];

public:

	BoundingBox() : bEmpty(true), dirty(true) {}
	BoundingBox(QVector3D bMin, QVector3D bMax) : bmin(bMin), bmax(bMax), bEmpty(false), dirty(true) {}

	QVector3D getMin() const { return bmin; }
	QVector3D getMax() const { return bmax; }
	bool isEmpty() { return bEmpty; }

	void setMin(QVector3D bMin) {
		bmin = bMin;
		dirty = true;
		bEmpty = false;
	}
	void setMax(QVector3D bMax) {
		bmax = bMax;
		dirty = true;
		bEmpty = false;
	}
	void setMin(int i, double val) {
		if (i == 0) { bmin[0] = val; bEmpty = false; }
		else if (i == 1) { bmin[1] = val; bEmpty = false; }
			else if (i == 2) { bmin[2] = val; bEmpty = false; }
	}
	void setMax(int i, double val) {
		if (i == 0) { bmax[0] = val; bEmpty = false; }
		else if (i == 1) { bmax[1] = val; bEmpty = false; }
			else if (i == 2) { bmax[2] = val; bEmpty = false; }
	}
	void setEmpty() {
		bEmpty = true;
	}

    // // Does this bounding box intersect the target?
    // bool intersects(const BoundingBox &target) const {
    //     return ((target.getMin()[0] - RAY_EPSILON <= bmax[0]) && (target.getMax()[0] + RAY_EPSILON >= bmin[0]) &&
    //             (target.getMin()[1] - RAY_EPSILON <= bmax[1]) && (target.getMax()[1] + RAY_EPSILON >= bmin[1]) &&
    //             (target.getMin()[2] - RAY_EPSILON <= bmax[2]) && (target.getMax()[2] + RAY_EPSILON >= bmin[2]));
    // }
    //
	// // does the box contain this point?
	// bool intersects(const QVector3D& point) const {
	// 	return ((point[0] + RAY_EPSILON >= bmin[0]) && (point[1] + RAY_EPSILON >= bmin[1]) && (point[2] + RAY_EPSILON >= bmin[2]) &&
	// 		(point[0] - RAY_EPSILON <= bmax[0]) && (point[1] - RAY_EPSILON <= bmax[1]) && (point[2] - RAY_EPSILON <= bmax[2]));
	// }
    //
	// // if the ray hits the box, put the "t" value of the intersection
	// // closest to the origin in tMin and the "t" value of the far intersection
	// // in tMax and return true, else return false.
	// // Using Kay/Kajiya algorithm.
	// bool intersect(const ray& r, double& tMin, double& tMax) const {
	// 	QVector3D R0 = r.getPosition();
	// 	QVector3D Rd = r.getDirection();
	// 	tMin = -1.0e308; // 1.0e308 is close to infinity... close enough for us!
	// 	tMax = 1.0e308;
	// 	double ttemp;
	//
	// 	for (int currentaxis = 0; currentaxis < 3; currentaxis++) {
	// 		double vd = Rd[currentaxis];
	// 		// if the ray is parallel to the face's plane (=0.0)
	// 		if( vd == 0.0 ) continue;
	// 		double v1 = bmin[currentaxis] - R0[currentaxis];
	// 		double v2 = bmax[currentaxis] - R0[currentaxis];
	// 		// two slab intersections
	// 		double t1 = v1/vd;
	// 		double t2 = v2/vd;
	// 		if ( t1 > t2 ) { // swap t1 & t2
	// 			ttemp = t1;
	// 			t1 = t2;
	// 			t2 = ttemp;
	// 		}
	// 		if (t1 > tMin) tMin = t1;
	// 		if (t2 < tMax) tMax = t2;
	// 		if (tMin > tMax) return false; // box is missed
	// 		if (tMax < RAY_EPSILON) return false; // box is behind ray
	// 	}
	// 	 return true; // it made it past all 3 axes.
	// }

    bool intersectSphere(QVector3D center, double radius) {
        if (dirty) updateCorners();

        // check if the center is inside the bounding box
        if (center.x() >= bmin.x() && center.y() >= bmin.y() && center.z() >= bmin.z()) 
            if (center.x() <= bmax.x() && center.y() <= bmax.y() && center.z() <= bmax.z()) 
                return true;
        
        // center is outside of the bounding box, then check if 
        // there is any possibility to intersect with corners
        int count = 0;
        for (int i = 0; i < 8; i++) {
            double distance = (center - corners[i]).length();
            if (distance < radius) return true;
        }
        return false;
    }

	void operator=(const BoundingBox& target) {
		bmin = target.bmin;
		bmax = target.bmax;
		bArea = target.bArea;
		bVolume = target.bVolume;
		dirty = target.dirty;
		bEmpty = target.bEmpty;
	}

	double area() {
		if (bEmpty) return 0.0;
		else if (dirty) {
			// bArea = 2.0 * ((bmax[0] - bmin[0]) * (bmax[1] - bmin[1]) + (bmax[1] - bmin[1]) * (bmax[2] - bmin[2]) + (bmax[2] - bmin[2]) * (bmax[0] - bmin[0]));
            updateCorners();
			dirty = false;
		}
		return bArea;
	}

	double volume() {
		if (bEmpty) return 0.0;
		else if (dirty) {
			// bVolume = ((bmax[0] - bmin[0]) * (bmax[1] - bmin[1]) * (bmax[2] - bmin[2]));
            updateCorners();
			dirty = false;
		}
		return bVolume;
	}

	void merge(const BoundingBox& bBox)	{
		if (bBox.bEmpty) return;
		for (int axis = 0; axis < 3; axis++) {
			if (bEmpty || bBox.bmin[axis] < bmin[axis]) bmin[axis] = bBox.bmin[axis];
			if (bEmpty || bBox.bmax[axis] > bmax[axis]) bmax[axis] = bBox.bmax[axis];
		}
		dirty = true;
		bEmpty = false;
	}

    void updateCorners() {
        double length = bmax.x() - bmin.x();
        double width  = bmax.z() - bmin.z();
        double height = bmax.y() - bmin.y();

        double x = bmin.x();
        double y = bmin.y();
        double z = bmin.z();

        // bottom 4 corners
        corners[0] = bmin;
        corners[1] = QVector3D(x + length, y, z);
        corners[2] = QVector3D(x, y, z + width);
        corners[3] = QVector3D(x + length, y, z + width);

        // top 4 corners
        corners[4] = QVector3D(x, y + height, z);
        corners[5] = QVector3D(x, y + height, z + width);
        corners[6] = QVector3D(x + length, y + height, z);
        corners[7] = bmax;

        bArea = 2.0 * ((bmax[0] - bmin[0]) * (bmax[1] - bmin[1]) + (bmax[1] - bmin[1]) * (bmax[2] - bmin[2]) + (bmax[2] - bmin[2]) * (bmax[0] - bmin[0]));
        bVolume = ((bmax[0] - bmin[0]) * (bmax[1] - bmin[1]) * (bmax[2] - bmin[2]));

        dirty = false;
    }

    QVector3D* getCorners() {
        if (dirty) updateCorners() ;
        return corners;
    }
};


#endif /* end of include guard: BOUNDINGBOX_H */
