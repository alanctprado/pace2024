' Copyright 2024, Gurobi Optimization, LLC */
'
' This example reads a model from a file and tunes it.
' It then writes the best parameter settings to a file
' and solves the model using these parameters.

Imports System
Imports Gurobi

Class tune_vb
    Shared Sub Main(ByVal args As String())

        If args.Length < 1 Then
            Console.Out.WriteLine("Usage: tune_vb filename")
            Return
        End If

        Try
            Dim env As New GRBEnv()

            ' Read model from file
            Dim model As New GRBModel(env, args(0))

            ' Set the TuneResults parameter to 2
            '
            ' The first parameter setting is the result for the first solved
            ' setting. The second entry the parameter setting of the best
            ' parameter setting.
            model.Parameters.TuneResults = 2

            ' Tune the model
            model.Tune()

            ' Get the number of tuning results
            Dim resultcount As Integer = model.TuneResultCount

            If resultcount >= 2 Then

                ' Load the tuned parameters into the model's environment
                '
                ' Note, the first parameter setting is associated to the first
                ' solved setting and the second parameter setting to best tune
                ' result.
                model.GetTuneResult(1)

                ' Write the tuned parameters to a file
                model.Write("tune.prm")

                ' Solve the model using the tuned parameters
                model.Optimize()
            End If

            ' Dispose of model and environment
            model.Dispose()
            env.Dispose()

        Catch e As GRBException
            Console.WriteLine("Error code: " & e.ErrorCode & ". " & e.Message)
        End Try
    End Sub
End Class
