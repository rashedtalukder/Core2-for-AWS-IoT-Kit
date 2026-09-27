# SPDX-FileCopyrightText: 2022 Rashed Talukder
# SPDX-License-Identifier: Apache-2.0

idf_component_register( SRCS "smoke.c"
                        REQUIRES Core2-for-AWS-IoT-Kit nvs_flash )