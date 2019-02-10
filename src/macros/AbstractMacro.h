/*
 * AbstractMacro.h
 *
 *  Created on: 10 févr. 2019
 *      Author: Fabien-B
 */

#ifndef SRC_MACROS_ABSTRACTMACRO_H_
#define SRC_MACROS_ABSTRACTMACRO_H_

namespace armothy{
class Armothy;

class AbstractMacro {
public:
	AbstractMacro(Armothy * arm);
	virtual ~AbstractMacro();

	void setup(Armothy * arm);

	virtual void init() = 0;

	// return: True if ended
	virtual bool  doIt() = 0;

	virtual void leave() = 0;

protected:
	Armothy* _armothy;
};
}

#endif /* SRC_MACROS_ABSTRACTMACRO_H_ */
