const { BlobServiceClient } = require("@azure/storage-blob");
const blobSasUrl = "https://storageaccounttest9163.blob.core.windows.net/?sv=2020-08-04&ss=bfqt&srt=sco&sp=rwdlacupix&se=2021-12-23T17:11:26Z&st=2021-12-23T09:11:26Z&spr=https&sig=XqQCGpionc8jGRTtiXJ6PRTKoYeljp1t%2BWwxu9upUCw%3D";
// Create a new BlobServiceClient
const blobServiceClient = new BlobServiceClient(blobSasUrl);
const { v1: uuidv1} = require('uuid');
const containerName = 'audios';
// Create a unique name for the container by 
// appending the current time to the file name

export async function uploadtoBlob(data) {
    // Get a reference to a container
    const containerClient = blobServiceClient.getContainerClient(containerName);
    
    // Create a unique name for the blob
    const blobName = data + uuidv1() + '.txt';

    // Get a block blob client
    const blockBlobClient = containerClient.getBlockBlobClient(blobName);

    const uploadBlobResponse = await blockBlobClient.upload(data, data.length);
    return uploadBlobResponse.requestId;
    // List the blob(s) in the container.
}