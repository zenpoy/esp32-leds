#include "fs_manager.h"

#include "SPIFFS.h"

bool FsManager::setup()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }

  return true;
}

bool FsManager::SaveToFs(const char *path, const uint8_t *payload, unsigned int length)
{
  Serial.println("writing file to FS");
  File file = SPIFFS.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("There was an error opening the file for writing");
    return false;
  }

  size_t bytesWritten = file.write(payload, length);
  if (bytesWritten != length)
  {
    Serial.println("did not write all bytes to file");
    return false;
  }
  file.close();
  return true;
}

unsigned int FsManager::ReadFromFs(const char *path, uint8_t *buffer, unsigned int length)
{
  File file = SPIFFS.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return 0;
  }
  unsigned int bytesRead = file.read(buffer, length);
  file.close();
  return bytesRead;
}
