#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <QObject>

class ICommand {
public:
  virtual ~ICommand() = default;
  
  // Método principal de ejecución.
  virtual void execute() = 0;
};

#endif
