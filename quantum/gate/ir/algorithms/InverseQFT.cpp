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
#include "InverseQFT.hpp"
#include "QFT.hpp"
#include "GateFunction.hpp"

namespace xacc {

namespace quantum {

std::shared_ptr<Function> InverseQFT::generateAlgorithm(std::vector<int> qubits) {

	// Create a function to return
	auto retFunction = std::make_shared<GateFunction>("inverse_qft");

	// Generate the QFT algorithm and get the
	// individual instructions
	QFT qft;
	auto qftFunction = qft.generateAlgorithm(qubits);
	auto instructions = qftFunction->getInstructions();

	// Reverse those instructions
	std::reverse(instructions.begin(), instructions.end());

	// Add them to the return function
	for (auto i : instructions) {
		retFunction->addInstruction(i);
	}

	return retFunction;
}

}
}

