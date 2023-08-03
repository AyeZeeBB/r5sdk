#------------------------------------------------------------------------------
import os
import sys
import json
import hashlib

#------------------------------------------------------------------------------
# Compute the SHA-256 checksum of a file
#------------------------------------------------------------------------------
def ComputeChecksum(filePath, blockSize=65536):
    sha256 = hashlib.sha256()
    with open(filePath, "rb") as file:
        for block in iter(lambda: file.read(blockSize), b""):
            sha256.update(block)

    checksum = sha256.hexdigest()
    print(f"*** computed checksum for '{filePath}': {checksum}")

    return checksum

#------------------------------------------------------------------------------
# Compute checksums for all files in a directory
#------------------------------------------------------------------------------
def RecursiveComputeChecksum(directoryPath, settings):
    checksums = {}
    scriptPath = os.path.abspath(__file__)

    for root, _, files in os.walk(directoryPath):
        for fileName in files:
            filePath = os.path.join(root, fileName)
            relativePath = os.path.relpath(filePath, directoryPath)
            normalizedPath = relativePath.replace("\\", "/")

            # Exclude the script itself from the manifest
            if os.path.abspath(filePath) == scriptPath:
                continue

            checksum = ComputeChecksum(filePath)
            checksums[normalizedPath] = {
                "checksum": checksum,
                "restart": False
            }

    # Check if the file should be updated/installed during a restart
    if settings and "restart" in settings:
        restart_files = settings["restart"]
        for file in restart_files:
            if file in checksums:
                checksums[file]["restart"] = True

    return checksums


#------------------------------------------------------------------------------
# Gets the settings for this depot
#------------------------------------------------------------------------------
def GetDepotSettings(depotName):
    settingsPath = os.path.join('devops', f'{depotName}.json')
    print(f"{settingsPath}")
    if not os.path.isfile(settingsPath):
        settingsPath = os.path.join('devops', 'default.json')
        if not os.path.isfile(settingsPath):
            return None

    f = open(settingsPath)
    if f.closed:
        print(f"{settingsPath} = closed")
        return None

    return json.load(f)


#------------------------------------------------------------------------------
# Save the checksums to a manifest file
#------------------------------------------------------------------------------
def CreateManifest(version, currentDirectory, outManifestFile):
    depotList = [name for name in os.listdir(currentDirectory) if os.path.isdir(os.path.join(currentDirectory, name))]

    manifest = {
        "version": version,
        "depot": {}
    }

    i = 0
    for depot in depotList:
        fileName = f"{depot}.zip"

        # Each depot must have a release package !!!
        zipFilePath = os.path.join(currentDirectory, 'release', fileName)

        print(f"{zipFilePath}")
        if not os.path.isfile(zipFilePath):
            continue

        # Each depot must have a configuration !!!
        settings = GetDepotSettings(depot)
        if not settings:
            continue

        print(f"*** processing depot '{depot}'...")

        checksum = ComputeChecksum(zipFilePath)
        size = os.path.getsize(zipFilePath)

        depotData = {
            "name": fileName,
            "size": size,
            "checksum": checksum,
            "optional": False,
            "vendor": settings["vendor"],
            "assets": RecursiveComputeChecksum(os.path.join(currentDirectory, depot), settings)
        }

        manifest["depot"][i] = depotData
        i+=1

    with open(outManifestFile, "w") as jsonFile:
        json.dump(manifest, jsonFile, indent=4)

#------------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: bld_man.py <version>")
        sys.exit(1)

    version = sys.argv[1]

    currentDirectory = os.path.dirname(os.path.abspath(__file__))
    outManifestFile = "manifest_patch.json"

    CreateManifest(version, currentDirectory, outManifestFile)
