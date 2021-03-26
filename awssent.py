# sending image to aws s3 bucket.

import sys
sys.path.append("./libboto3")            # Adding dependency library path 
import config
import boto3
import numpy as np


s3_client = boto3.client('s3', aws_access_key_id = config.aws_access_key_id, aws_secret_access_key = config.aws_secret_access_key , region_name= config.region)

s3_client.upload_file('image.jpg', config.bucket_name , 'image_upload.jpg')
