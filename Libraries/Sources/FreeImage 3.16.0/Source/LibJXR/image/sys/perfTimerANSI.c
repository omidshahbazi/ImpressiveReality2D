īf?ļ7@ ü*       `Û     	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙       ~          	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙       ~          	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙       ~          	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙       ~          	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                 	   ˙                                                       ŊŊeŊJŊ/ŊŊųŧŪŧÃŧ¨ŧŧrŧWŧ<ŧ!ŧŧëģĐģĩģģģdģIģ.ģģøēŨēÂē§ēēqēVē;ē ēēęšĪš´šš~šcšHš-šš÷¸Ü¸Á¸Ļ¸¸p¸U¸:¸¸¸éˇÎˇŗˇˇ}ˇbˇGˇ,ˇˇöļÛļĀļĨļļoļTļ9ļļļčĩÍĩ˛ĩĩ|ĩaĩFĩ+ĩĩõ´Ú´ŋ´¤´´n´S´8´´´įŗĖŗąŗŗ{ŗ`ŗEŗ*ŗŗô˛Ų˛ž˛Ŗ˛˛m˛R˛7˛˛˛æąËą°ąązą_ąDą)ąąķ°Ø°Ŋ°ĸ°°l°Q°6°° ° the right state
    if (CS_STOPPED != pState->eState)
    {
        assert(FALSE);
        goto exit;
    }

    pState->iPrevStartTime = clock();

    // Check clock result
    if ((clock_t)-1 == pState->iPrevStartTime)
    {
        TraceResult(WM_E_CLOCKFAILURE);
        goto exit;
    }

    pState->eState = CS_RUNNING;
    fResult = TRUE;

exit:
    return fResult;
} // PerfTimerStart



Bool PerfTimerStop(PERFTIMERSTATE *pState)
{
    Bool        fResult = FALSE;

    if (NULL == pState)
    {
        // Can happen because we typically ignore errors and use a single bool to
        // control all perf timing (some of which can fail to init)
        goto exit;
    }

    // Make sure we are in the right state
    if (CS_RUNNING != pState->eState)
    {
        assert(FALSE);
        goto exit;
    }

    fResult = AccumulateTime(pState, &pState->iElapsedTime);
    pState->eState = CS_STOPPED;
    fResult = TRUE;

exit:
    return fResult;
} // PerfTimerStop



Bool PerfTimerGetResults(PERFTIMERSTATE *pState, PERFTIMERRESULTS *pResults)
{
    Bool            fResult = FALSE;
    PERFTIMERTIME   iElapsedTime;

    if (NULL == pState)
    {
        // Can happen because we typically ignore errors and use a single bool to
        // control all perf timing (some of which can fail to init)
        goto exit;
    }

    // Make sure we are in the right state
    if (CS_STOPPED != pState->eState && CS_RUNNING != pState->eState)
    {
        assert(FALSE);
        goto exit;
    }

    iElapsedTime = pState->iElapsedTime;
    if (CS_RUNNING == pState->eState)
    {
        // Must take a "checkpoint" time reading
        fResult = AccumulateTime(pState, &iElapsedTime);
        if (FALSE == fResult)
            goto exit;
    }

    // Convert clock ticks to nanoseconds.
    // Use floating point for ease of math. If your platform really blows
    // with floating point, replace this with appropriate integer calculation
    // based on your clock interval.
    pResults->iElapsedTime = (PERFTIMERTIME)((float)iElapsedTime *
        ((float)NANOSECONDS_PER_SECOND / (float)CLOCKS_PER_SEC));
    pResults->iTicksPerSecond = CLOCKS_PER_SEC;
    pResults->iZeroTimeIntervals = pState->iZeroTimeIntervals;
    fResult = TRUE;

exit:
    return fResult;
} // PerfTimerGetResults



Bool PerfTimerCopyStartTime(PERFTIMERSTATE *pDestPerfTimer,
                            PERFTIMERSTATE *pSrcPerfTimer)
{
    Bool    fResult = FALSE;

    if (NULL == pDestPerfTimer)
    {
        TraceResult(E_INVALIDARG);
        goto exit;
    }

    if (NULL == pSrcPerfTimer)
    {
        TraceResult(E_INVALIDARG);
        goto exit;
    }

    // Check that both timers are in proper state - both must be running
    if (CS_RUNNING != pDestPerfTimer->eState)
    {
        TraceResult(WM_E_INVALIDSTATE);
        goto exit;
    }

    if (CS_RUNNING != pSrcPerfTimer->eState)
    {
        TraceResult(WM_E_INVALIDSTATE);
        goto exit;
    }

    if (0 != pDestPerfTimer->iElapsedTime)
    {
        // If iElapsedTime is non-zero, caller won't get what he is expecting
        // when he calls PerfTimerGetResults
        TraceResult(WM_E_INVALIDSTATE);
        goto exit;
    }

    pDestPerfTimer->iPrevStartTime = pSrcPerfTimer->iPrevStartTime;
    fResult = TRUE;

exit:
    return fResult;
} // PerfTimerCopyStartTime

#endif // DISABLE_PERF_MEASUREMENT
