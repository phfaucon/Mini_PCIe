/*
 * IrqTest.cpp
 *
 *  Created on: Feb 11, 2021
 *      Author: alphi
 */

#include <iostream>
#include "CanFdTest.h"

ParallelInput *irqStat;
int irqNbr = 0;
int rxCharCnt1;
int rxCharCnt2;
int intEnabled = 0;


void isrTest(void *userData)
{
// if (irqNbr == 0)
// std::cout << "Received Irq!\n";
Sleep(100);
irqNbr++;
irqStat->setIrqDisable(0xffff);
irqStat->setIrqEnable(0xffff);
}

int CanFdTest::staticIrqTest() {
// SPI status address
irqStat = dut->input0;
irqNbr = 0;

// PCIe controller registers
// volatile UINT32 * PCIeIrqStatus = dut->devAddressBar0(0x40);
//volatile UINT32 * PCIeIrqEnable = dut->devAddressBar0(0x50);
// cout << "PCIe IRQ status: " << hex << "0x" << *PCIeIrqStatus << endl;

// enable the interrupts in the PCIe controller
// hook the ISR
dut->hookInterruptServiceRoutine(isrTest);
dut->enableInterrupts(0xffff);

// irqStat->reset();
irqStat->setIrqEnable(0xffff);
// irqStat->setDirection(0xffff);
irqStat->setDataOut(0xffff);
irqStat->setDataOut(0x0);

Sleep(1000);

std::cout << "PCIe IRQ status: " << std::hex << "0x" << dut->cra->getIrqStatus() << std::endl;
std::cout << "PCIe IRQ enable: " << std::hex << "0x" << dut->cra->getIrqEnableMask() << std::endl;
//

// test the ARINC receive interrupt
// program the receiver transmitter
// program the SPIStatus to receive just the IRQ
irqStat->setEdgeCapture(0xffff);
// edge only
// send a character on the loopback
// have we received the interrupt?

// cout << "PCIe IRQ status: " << hex << "0x" << *PCIeIrqStatus << endl;
dut->unhookInterruptServiceRoutine();
irqStat->setIrqDisable(0xffff);
dut->enableInterrupts(0);

if (irqNbr >= 1)
{
	std::cout << "*** Static PCIe Interrupt test: PASSED" << std::endl;
}
else {
	std::cout << "*** Static PCIe Interrupt test: FAILED" << std::endl;
}
dut->reset();
return (irqNbr == 0);
}

