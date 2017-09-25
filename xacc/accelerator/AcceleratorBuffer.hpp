/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_
#define XACC_ACCELERATOR_ACCELERATORBUFFER_HPP_

#include <boost/dynamic_bitset.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include "Utils.hpp"

namespace xacc {

// Our Accelerator Bits can be a 1, 0, or unknown
enum class AcceleratorBitState {
	ZERO, ONE, UNKNOWN
};

/**
 * The AcceleratorBit wraps an AcceleratorBitSate
 * and provides a mechanism for updating that state.
 *
 * @author Alex McCaskey
 */
class AcceleratorBit {

public:

	/**
	 * The constructor, all bits are initialized to unknown state
	 */
	AcceleratorBit() :
			state(AcceleratorBitState::UNKNOWN) {
	}

	/**
	 * Update the Bit state to a one or zero
	 */
	void update(int zeroOrOne) {
		state = (
				zeroOrOne == 0 ?
						AcceleratorBitState::ZERO : AcceleratorBitState::ONE);
	}

	/**
	 * Return the value of this state
	 */
	AcceleratorBitState getState() {
		return state;
	}

protected:

	/**
	 * The bit state
	 */
	AcceleratorBitState state;
};

/**
 * The AcceleratorBuffer models an allocated buffer of
 * bits that are operated on by a kernel. As such,
 * the AcceleratorBuffer's primary role is to store
 * Accelerator execution results.
 *
 * @author Alex McCaskey
 */
class AcceleratorBuffer {

public:

	/**
	 * The Constructor
	 */
	AcceleratorBuffer(const std::string& str, const int N) :
			bufferId(str), bits(std::vector<AcceleratorBit>(N)) {
	}

	/**
	 * The Constructor, takes as input the name of this buffer,
	 * and the bit indices to model.
	 *
	 * @param str The name of the buffer
	 * @param firstIndex The first bit index
	 * @param indices The remaining bit indices
	 */
	template<typename ... Indices>
	AcceleratorBuffer(const std::string& str, int firstIndex,
			Indices ... indices) :
			bufferId(str), bits(
					std::vector<AcceleratorBit>(1 + sizeof...(indices))) {
	}

	/**
	 * Return the number of bits in this buffer.
	 *
	 * @return size The number of bits in this buffer
	 */
	const int size() const {
		return bits.size();
	}

	/**
	 * Return this AcceleratorBuffer's name
	 *
	 * @return name The name of this AcceleratorBuffer
	 */
	const std::string name() const {
		return bufferId;
	}

	/**
	 * Reset the stored measured bit strings.
	 */
	virtual void resetBuffer() {
		measurements.clear();
	}

	/**
	 * Update the Bit State for the bit at the give index
	 *
	 * @param idx The index of the bit
	 * @param zeroOrOne The measurement result
	 */
	void updateBit(const int idx, int zeroOrOne) {
		bits[idx].update(zeroOrOne);
	}

	/**
	 * Add a measurement result to this Buffer
	 *
	 * @param measurement The measurement result
	 */
	virtual void appendMeasurement(const boost::dynamic_bitset<>& measurement) {
		measurements.push_back(measurement);
	}

	/**
	 * Compute and return the expectation value with respect
	 * to the Pauli-Z operator. Here we provide a base implementation
	 * based on an ensemble of measurement results. Subclasses
	 * are free to implement this as they see fit, ie, for simulators
	 * use the wavefunction.
	 *
	 * @return expVal The expectation value
	 */
	virtual const double getExpectationValueZ() {
		std::stringstream ss;
		double aver = 0.;
		long n_measurements = measurements.size();
		unsigned long tmp;
		bool odd;
		for (unsigned long bucket = 0; bucket < (1UL << measurements[0].size());
				++bucket) {
			long count = 0;      // use bucket to "collect"(i.e. count)
			for (const auto outcome : measurements) {
				if (outcome.to_ulong() == bucket) {
					count++;
				}
			}
			double p = double(count) / n_measurements;
			ss << "p= " << p << std::endl;
			tmp = bucket;
			odd = false;
			while (tmp) {
				odd = !odd;
				tmp = tmp & (tmp - 1);
			}

			if (!odd) {
				aver += p;
			} else {
				aver -= p;
			}
		}
		return aver;
	}

	/**
	 * Return the bit state for the given bit.
	 *
	 * @param idx The index of the bit
	 * @return bitState the state of the bit, a 0, 1, or UNKNOWN
	 */
	AcceleratorBitState getAcceleratorBitState(const int idx) {
		return bits[idx].getState();
	}

	/**
	 * Print information about this AcceleratorBuffer to standard out.
	 *
	 */
	virtual void print() {
	}

	/**
	 * Print information about this AcceleratorBuffer to the
	 * given output stream.
	 *
	 * @param stream Stream to write the buffer to.
	 */
	virtual void print(std::ostream& stream) {
	}

	/**
	 * The Destructor
	 */
	virtual ~AcceleratorBuffer() {
	}

protected:

	/**
	 * Reference to the Accelerator measurement result bit strings
	 */
	std::vector<boost::dynamic_bitset<>> measurements;

	/**
	 * The name of this AcceleratorBuffer
	 */
	std::string bufferId;

	/**
	 * The set of AcceleratorBits.
	 */
	std::vector<AcceleratorBit> bits;
};

}

#endif
